//
//  PureParser.cpp
//  PureParser
//
//  Copyright © 2019 JivoSite Inc. All rights reserved.
//  <For detailed info about how this parser works, please refer to README.md file>
//

#include "PureParser.hpp"
#include "PureScanner.hpp"
#include <regex>
#include <iostream>
#include <optional>
#include <list>

const char * const kPureParserDefaultElementToken = "$";
const char * const kPureParserDefaultBlockOpenerToken = "[";
const char * const kPureParserDefaultBlockCloserToken = "]";
const char * const kPureParserDefaultSeparatorToken = "##";
const char * const kPureParserDefaultAliasToken = ":";

static bool is_valuable_symbol(char symbol);

PureParser::PureParser(PureConfig config) {
    this->_config = config;
}

void PureParser::reset() {
    _assigned_variables.clear();
    _enabled_aliases.clear();
}

void PureParser::assignVariable(std::string name, std::string value) {
    _assigned_variables.insert_or_assign(name, value);
}

void PureParser::discardVariable(std::string name) {
    _assigned_variables.erase(name);
}

void PureParser::enableAlias(std::string name) {
    _enabled_aliases.insert(name);
}

void PureParser::disableAlias(std::string name) {
    _enabled_aliases.erase(name);
}

std::string PureParser::execute(std::string formula, bool collapse_spaces, bool reset_on_finish) {
    // Parse the entire formula like a root frame into a tree
    size_t frame_len = 0;
    const std::optional<PureElement> frame = recognizeFrame(formula, &frame_len);

    // Resolve the tree into the output,
    // or use empty string if something went wrong
    std::string result = frame.has_value()
        ? resolveFrame(*frame).value_or(std::string())
        : std::string();

    // Whether we should discard all variables and alises
    // right after the formula was executed; in preparing for next execution
    if (reset_on_finish) {
        reset();
    }

    // Whether we should remove all extra spaces from the output
    if (collapse_spaces) {
        return removeExtraSpaces(result);
    }
    else {
        return result;
    }
}

std::optional<PureElement> PureParser::recognizeFrame(std::string input, size_t *scanned_len) {
    PureScanner scanner(input);
    std::list<PureElement> children_elements;
    std::optional<std::string> alias_name;
    bool any_symbol_scanned = false;
    
    // The main scanning loop with some details below:
    *scanned_len = 0;
    while (scanner.canContinue()) {
        // If scanner currently scans the alias,
        // which means the `alias_name` is defined as empty string
        if (alias_name.has_value() && alias_name->length() == 0) {
            // (2) If scanner points the alias token again,
            // it means we have just parsed the alias name
            if (scanner.detectAndSlice(_config.alias_token)) {
                // Save the already captured alias name
                alias_name = scanner.last_slice;
                *scanned_len += alias_name->length() + _config.alias_token.length();
            }
            // (1) Otherwise, just step forward;
            // in waiting for alias would be entirely captured
            else {
                scanner.lookBy(1);
                continue;
            }
        }
        else {
            // (0) If scanner points to alias token at first time,
            // and no alias or any elements were prevously captured
            if (not alias_name.has_value() && children_elements.empty() && not any_symbol_scanned && scanner.detectAndSlice(_config.alias_token)) {
                // Indicate the `alias_name` as empty string instead of being `null`
                // to detect that we have started the alias name scanning
                alias_name = std::string();
                
                // Add the already captured slice into children elements
                children_elements.emplace_back(PureElementTypeSlice, scanner.last_slice);
                *scanned_len += scanner.last_slice.length() + _config.alias_token.length();
            }
            // (2) Is scanner points to element token,
            // this should be captured into children elements
            else if (scanner.detectAndSlice(_config.element_token)) {
                // Add the already captured slice into children elements
                children_elements.emplace_back(PureElementTypeSlice, scanner.last_slice);
                *scanned_len += scanner.last_slice.length();
                *scanned_len += _config.element_token.length();

                // Attempt to recognize an element from just captured slice
                size_t element_len = 0;
                const auto element = recognizeElement(scanner.following_slice, &element_len);
                *scanned_len += element_len;

                // If an element has been recognized, place it into children elements
                if (element.has_value()) {
                    children_elements.push_back(*element);
                    scanner.skipBy(element_len);
                }
                // Otherwise, place just the element token itself into children elements
                else {
                    children_elements.emplace_back(PureElementTypeSlice, _config.element_token);
                }
            }
            // (3) If scanner points to separator token, then
            // we have to construct and return our resulting frame
            else if (scanner.detectAndSlice(_config.separator_token)) {
                // Place the rest of contents into children elements
                const std::string payload = alias_name.value_or(std::string());
                children_elements.emplace_back(PureElementTypeSlice, scanner.last_slice);
                *scanned_len += scanner.last_slice.length();
                return PureElement(PureElementTypeFrame, payload, children_elements);
            }
            // No especial tokens were found, so just continue looking
            else {
                if (not any_symbol_scanned) {
                    any_symbol_scanned = scanner.detectWithCallback(is_valuable_symbol);
                }
                
                scanner.lookBy(1);
                continue;
            }
        }
    }

    // If scanner have reached the end, then
    // we have to construct and return our resulting frame;
    // next to placing the rest of contents into children elements
    const std::string payload = alias_name.value_or(std::string());
    children_elements.emplace_back(PureElementTypeSlice, scanner.following_slice);
    *scanned_len += scanner.following_slice.length();
    return PureElement(PureElementTypeFrame, payload, children_elements);
}

std::optional<PureElement> PureParser::recognizeElement(std::string input, size_t *scanned_len) {
    // First, try to recognize the element as block
    std::optional<PureElement> element = recognizeBlockElement(input, scanned_len);
    if (element.has_value()) {
        return element;
    }

    // If not, try to recognize it as variable
    element = recognizeVariableElement(input, scanned_len);
    if (element.has_value()) {
        return element;
    }

    // If not, indicate the failure
    return std::nullopt;
}

std::optional<PureElement> PureParser::recognizeBlockElement(std::string input, size_t *scanned_len) {
    // If input does not start with block opener token, it's not the block
    if (input.find(_config.block_opener_token) != 0) {
        return std::nullopt;
    }

    PureScanner scanner(input);
    std::list<PureElement> block_frames;

    // First, extract the block payload that is between opener and closer tokens,
    if (scanner.detectAndExtract(_config.block_opener_token, _config.block_closer_token)) {
        // Inform in advance about the just scanned amount
        *scanned_len = _config.block_opener_token.length() + scanner.last_slice.length() + _config.block_closer_token.length();
        
        // Refill the scanner with only payload we actually need
        scanner.reset(scanner.last_slice);
    }
    // If payload was not extracted, it has to be an invalid block,
    // so we should indicate the failure
    else {
        return std::nullopt;
    }

    // Perform the scanning while we have the data to scan
    while (scanner.canContinue()) {
        // Attempt to recognize the current frame
        size_t frame_len;
        std::optional<PureElement> frame = recognizeFrame(scanner.input, &frame_len);

        // If frame is correct, append in into block frames
        // and point the scanner into place after this frame
        if (frame.has_value() && frame_len > 0) {
            block_frames.push_back(*frame);
            scanner.skipBy(frame_len);
        }

        // If we met the separator, then skip it
        // and proceed to the next frame recognition
        if (scanner.detectAndSlice(_config.separator_token)) {
            continue;
        }
        // If no separator found, then we have reached the end of block,
        // so stop the scanning and return the block
        else {
            break;
        }
    }

    return PureElement(PureElementTypeBlock, std::string(), block_frames);
}

std::optional<PureElement> PureParser::recognizeVariableElement(std::string input, size_t *scanned_len) {
    // Allowed symbols are: letters, digits, underscore
    const auto isAllowedSymbol = [](char symbol) -> bool {
        return (isalpha(symbol) || isdigit(symbol) || (symbol == '_'));
    };

    // If input does not start with allowed symbol, it's not a variable
    const auto begin = input.begin();
    if (!isAllowedSymbol(*begin)) {
        return std::nullopt;
    }

    // While scanning the input...
    auto iter = begin;
    while (iter < input.end()) {
        // Pass the allowed symbols
        if (isAllowedSymbol(*iter)) {
            iter++;
            continue;
        }

        // When an non-allowed symbol was found, stop the scanning
        // and use the just captured symbols as variable name
        const size_t variable_len = iter - begin;
        const std::string variable = input.substr(0, variable_len);
        *scanned_len = variable_len;
        return PureElement(PureElementTypeVariable, variable);
    }

    // If the loop reached the end,
    // then the entire input acts like valid variable name
    *scanned_len = input.length();
    return PureElement(PureElementTypeVariable, input);
}

std::optional<std::string> PureParser::resolveFrame(const PureElement &frame) {
    // If the frame has alias, and this alias is not activated,
    // the frame should be skipped as invalid
    const std::string alias = frame.payload;
    if (not alias.empty() && _enabled_aliases.find(alias) == _enabled_aliases.end()) {
        return std::nullopt;
    }

    // To resolve a frame,
    // we need to join all its valid elements into single output
    std::string output;
    for (const auto &element : frame.children) {
        const std::optional<std::string> element_output = resolveElement(element);
        if (element_output.has_value()) {
            output += *element_output;
        }
        // But, if any children element is invalid,
        // the entire frame has to be invalid as well
        else {
            return std::nullopt;
        }
    }

    return output;
}

std::optional<std::string> PureParser::resolveSlice(const PureElement &slice) {
    // To resolve a slice,
    // we just have to return its contents
    return slice.payload;
}

std::optional<std::string> PureParser::resolveElement(const PureElement &element) {
    // Resolve the element in a proper way
    // accordingly to its type
    switch (element.type) {
        case PureElementTypeFrame: return resolveFrame(element);
        case PureElementTypeBlock: return resolveBlockElement(element);
        case PureElementTypeVariable: return resolveVariableElement(element);
        case PureElementTypeSlice: return resolveSlice(element);
    }
}

std::optional<std::string> PureParser::resolveBlockElement(const PureElement &block) {
    // To resolve a block,
    // we need return its first valid element
    for (const auto &element : block.children) {
        const std::optional<std::string> element_output = resolveElement(element);
        if (element_output.has_value()) {
            return *element_output;
        }
    }
    
    // If no valid element was found, use the empty string
    return std::string();
}

std::optional<std::string> PureParser::resolveVariableElement(const PureElement &variable) {
    // To resolve a block,
    // we need to obtain its assigned value at first
    const std::string variable_name = variable.payload;
    const auto variable_iter = _assigned_variables.find(variable_name);

    // If the value is not assigned, the variable is invalid;
    // otherwise, it is
    if (variable_iter == _assigned_variables.end()) {
        return std::nullopt;
    }
    else {
        return variable_iter->second;
    }
}

std::string PureParser::removeExtraSpaces(std::string string) {
    // RegEx to remove the double spaces from within the output
    const std::regex collapse_expr("\\s+(?:(\\s|[.?!;:,])|$)");
    std::smatch collapse_match;

    // Keep in loop while the next processed output differs from its input
    std::string collapsed = string;
    while (true) {
        const std::string result = std::regex_replace(collapsed, collapse_expr, "$1");
        if (result == collapsed) break;
        collapsed = result;
    }

    // Return the output without leading and trailing extra spaces
    const auto _spaceDetector = [](int symbol) -> bool {
        return std::isspace(symbol);
    };
    
    const auto front_begin = collapsed.cbegin();
    const auto front_end = collapsed.cend();
    const auto front_iter = std::find_if_not(front_begin, front_end, _spaceDetector);
    
    const auto back_begin = collapsed.crbegin();
    const auto back_end = collapsed.crend();
    const auto back_iter = std::find_if_not(back_begin, back_end, _spaceDetector).base();
    
    if (front_iter < back_iter) {
        return std::string(front_iter, back_iter);
    }
    else {
        return std::string();
    }
}

static bool is_valuable_symbol(char symbol) {
    return not isspace(symbol);
}
