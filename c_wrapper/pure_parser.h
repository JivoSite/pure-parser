//
//  pure_parser.hpp
//  PureParser
//
//  Copyright © 2019 JivoSite Inc. All rights reserved.
//  <For detailed info about how this parser works, please refer to README.md file>
//

#ifndef pure_parser_h
#define pure_parser_h

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 Parser configuration with tokens to use when parsing formulas
*/
typedef struct {
    const char *element_token;
    const char *block_opener_token;
    const char *block_closer_token;
    const char *separator_token;
    const char *alias_token;
} pure_config_t;

/*
 Adjust the parser configuration for default usage
*/
void pure_config_set_default(pure_config_t *config);

/*
 Public parser context
*/
typedef struct {
    void *impl;
} pure_parser_t;

/*
 Memory management:
 - create the parser, at first
 - reset all variables & aliases, any time between different usages
 - release the memory, at the end
*/
void pure_parser_init(pure_parser_t *parser, const pure_config_t *config);
void pure_parser_reset(pure_parser_t *parser);
void pure_parser_destroy(pure_parser_t *parser);

/*
 Variables management:
 - assign the variable with value
 - discard the variable
*/
void pure_parser_assign_var(pure_parser_t *parser, const char *name, const char *value);
void pure_parser_discard_var(pure_parser_t *parser, const char *name);

/*
 Aliases management:
 - enable the alias
 - discard the alias
*/
void pure_parser_enable_alias(pure_parser_t *parser, const char *name);
void pure_parser_discard_alias(pure_parser_t *parser, const char *name);

/*
 Performing:
 - calculates the formula with assigned variables and enabled aliases into the output
*/
void pure_parser_perform(pure_parser_t *parser, const char *formula, bool collapse_spaces, bool reset_on_finish, char **output, unsigned long *output_len);

/*
 Performing:
 - calculates the formula with assigned variables and enabled aliases into the output
*/
void pure_parser_execute(pure_parser_t *parser, const char *formula, bool collapse_spaces, bool reset_on_finish, char **output, unsigned long *output_len);

#ifdef __cplusplus
}
#endif

#endif /* pure_parser_h */
