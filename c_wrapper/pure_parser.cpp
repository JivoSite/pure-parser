//
//  pure_parser.cpp
//  PureParser
//
//  Copyright © 2019 JivoSite Inc. All rights reserved.
//  <For detailed info about how this parser works, please refer to README.md file>
//

#include "pure_parser.h"
#include "../cpp_src/PureParser.hpp"
#include <regex>
#include <iostream>
#include <optional>

static inline PureParser* get_impl(pure_parser_t *parser);

void pure_config_set_default(pure_config_t *config) {
    config->element_token = kPureParserDefaultElementToken;
    config->block_opener_token = kPureParserDefaultBlockOpenerToken;
    config->block_closer_token = kPureParserDefaultBlockCloserToken;
    config->separator_token = kPureParserDefaultSeparatorToken;
    config->alias_token = kPureParserDefaultAliasToken;
}

void pure_parser_init(pure_parser_t *parser, const pure_config_t *config) {
    PureConfig impl_config {
        .element_token = config->element_token,
        .block_opener_token = config->block_opener_token,
        .block_closer_token = config->block_closer_token,
        .separator_token = config->separator_token,
        .alias_token = config->alias_token
    };

    parser->impl = new PureParser(impl_config);
}

void pure_parser_reset(pure_parser_t *parser) {
    get_impl(parser)->reset();
}

void pure_parser_destroy(pure_parser_t *parser) {
    delete get_impl(parser);
}

void pure_parser_assign_var(pure_parser_t *parser, const char *name, const char *value) {
    get_impl(parser)->assignVariable(name, value);
}

void pure_parser_discard_var(pure_parser_t *parser, const char *name) {
    get_impl(parser)->discardVariable(name);
}

void pure_parser_enable_alias(pure_parser_t *parser, const char *name) {
    get_impl(parser)->enableAlias(name);
}

void pure_parser_discard_alias(pure_parser_t *parser, const char *name) {
    get_impl(parser)->disableAlias(name);
}

void pure_parser_execute(pure_parser_t *parser, const char *formula, bool collapse_spaces, bool reset_on_finish, char **output, unsigned long *output_len) {
    const std::string result = get_impl(parser)->execute(formula, collapse_spaces, reset_on_finish);
    const size_t result_len = result.size();
    
    if (output_len) {
        *output_len = result_len;
    }
    
    if (result_len > 0) {
        *output = (char *) malloc(result_len + 1);
        memcpy(*output, result.data(), result_len);
        (*output)[result_len] = 0x00;
    }
    else {
        *output = NULL;
    }
}

static inline PureParser* get_impl(pure_parser_t *parser) {
    return (PureParser *) parser->impl;
}
