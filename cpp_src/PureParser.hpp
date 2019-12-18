//
//  PureParser.hpp
//  PureParser
//
//  Copyright © 2019 JivoSite Inc. All rights reserved.
//  <For detailed info about how this parser works, please refer to README.md file>
//

#ifndef PureParser_hpp
#define PureParser_hpp

#include "PureElement.hpp"
#include <string>
#include <map>
#include <set>
#include <utility>
#include <optional>

/**
 * By default, the config
 * uses these tokens for scanning
*/
extern const char * const kPureParserDefaultElementToken; // "$"
extern const char * const kPureParserDefaultBlockOpenerToken; // "["
extern const char * const kPureParserDefaultBlockCloserToken; // "]"
extern const char * const kPureParserDefaultSeparatorToken; // "##"
extern const char * const kPureParserDefaultAliasToken; // ":"

/**
 * The parsing configuration that you can redefine
 * to get your own behaviour of scanning process
 */
struct PureConfig {
    std::string element_token = kPureParserDefaultElementToken;
    std::string block_opener_token = kPureParserDefaultBlockOpenerToken;
    std::string block_closer_token = kPureParserDefaultBlockCloserToken;
    std::string separator_token = kPureParserDefaultSeparatorToken;
    std::string alias_token = kPureParserDefaultAliasToken;
};

/**
 * The PureParser itself
 */
class PureParser {
public:
    /**
     * Create the parser with the custom configuration,
     * or use the built-in one with standard tokens listed above
     */
    PureParser(PureConfig config = {});

    /**
     * Reset all variables and aliases being assigned between executes
     */
    void reset();

    /**
     * Variables management:
     * - assign the value to variable
     * - discard the variable
     */
    void assignVariable(std::string name, std::string value);
    void discardVariable(std::string name);

    /**
     * Alias management:
     * - enable the alias
     * - discard the alias
     */
    void enableAlias(std::string name);
    void disableAlias(std::string name);

    /**
     * Execute the formula with previously assigned variables and aliases
     */
    std::string execute(std::string formula, bool collapse_spaces, bool reset_on_finish);

private:
    std::optional<PureElement> recognizeFrame(std::string input, size_t *scanned_len);
    std::optional<PureElement> recognizeElement(std::string input, size_t *scanned_len);
    std::optional<PureElement> recognizeBlockElement(std::string input, size_t *scanned_len);
    std::optional<PureElement> recognizeVariableElement(std::string input, size_t *scanned_len);

    std::optional<std::string> resolveFrame(const PureElement &frame);
    std::optional<std::string> resolveSlice(const PureElement &slice);
    std::optional<std::string> resolveElement(const PureElement &element);
    std::optional<std::string> resolveBlockElement(const PureElement &block);
    std::optional<std::string> resolveVariableElement(const PureElement &variable);

    std::string removeExtraSpaces(std::string string);

private:
    PureConfig _config;
    std::map<std::string, std::string> _assigned_variables;
    std::set<std::string> _enabled_aliases;
};

#endif /* PureParser_hpp */
