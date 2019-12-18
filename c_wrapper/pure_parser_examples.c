//
//  pure_parser_examples.c
//  PureParser
//
//  Created by Stan Potemkin on 05.12.2019.
//

#include "pure_parser.h"
#include <stdio.h>
#include <string.h>

#pragma mark - Local Types

typedef struct {
    const char *formula;
    char variables[10][100];
    char aliases[10][100];
    char *output;
    const char *reference;
} example_meta_t;

typedef void(*example_func_t)(example_meta_t *);

typedef struct {
    const char *caption;
    example_func_t executor;
} example_t;

#pragma mark - Local Helpers

static example_t example_make(const char *caption, example_func_t executor) {
    example_t meta;
    meta.caption = caption;
    meta.executor = executor;
    return meta;
}

static void example_meta_set_formula(example_meta_t *meta, const char *formula) {
    meta->formula = formula;
}

static void example_meta_set_variable(example_meta_t *meta, size_t index, const char *name, const char *value) {
    const size_t base = index * 2;
    strcpy(meta->variables[base + 0], name);
    strcpy(meta->variables[base + 1], value);
    strcpy(meta->variables[base + 2], "");
}

static void example_meta_reset_variables(example_meta_t *meta) {
    strcpy(meta->variables[0], "");
}

static void example_meta_enable_alias(example_meta_t *meta, size_t index, const char *name) {
    const size_t base = index;
    strcpy(meta->aliases[base + 0], name);
    strcpy(meta->aliases[base + 1], "");
}

static void example_meta_reset_aliases(example_meta_t *meta) {
    strcpy(meta->aliases[0], "");
}

static void example_meta_set_reference(example_meta_t *meta, const char *reference) {
    meta->reference = reference;
}

#pragma mark - Examples

static void test_Plain(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "Hello world";
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_reset_aliases(meta);
    example_meta_reset_variables(meta);
    example_meta_set_reference(meta, "Hello world");
}

static void test_Variable(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "Hello, $name";
    pure_parser_assign_var(&parser, "name", "Stan");
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_reset_aliases(meta);
    example_meta_set_variable(meta, 0, "name", "Stan");
    example_meta_set_reference(meta, "Hello, Stan");
}

static void test_VariableAndBlockWithVariable(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "Please wait, $name: we're calling $[$anotherName ## another guy]";
    pure_parser_assign_var(&parser, "name", "Stan");
    pure_parser_assign_var(&parser, "anotherName", "Paul");
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_reset_aliases(meta);
    example_meta_set_variable(meta, 0, "name", "Stan");
    example_meta_set_variable(meta, 1, "anotherName", "Paul");
    example_meta_set_reference(meta, "Please wait, Stan: we're calling Paul");
}

static void test_VariableAndBlockWithPlain(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "Please wait, $name: we're calling $[$anotherName ## another guy]";
    pure_parser_assign_var(&parser, "name", "Stan");
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_reset_aliases(meta);
    example_meta_set_variable(meta, 0, "name", "Stan");
    example_meta_set_reference(meta, "Please wait, Stan: we're calling another guy");
}

static void test_BlockWithRich(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "Congrats! You saved it $[in folder '$folder'].";
    pure_parser_assign_var(&parser, "folder", "Documents");
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_reset_aliases(meta);
    example_meta_set_variable(meta, 0, "folder", "Documents");
    example_meta_set_reference(meta, "Congrats! You saved it in folder 'Documents'.");
}

static void test_InactiveBlock(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "Congrats! You saved it $[in folder '$folder'].";
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_reset_aliases(meta);
    example_meta_reset_variables(meta);
    example_meta_set_reference(meta, "Congrats! You saved it.");
}

static void test_ComplexActiveAlias(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "$[Agent $creatorName ## You] changed reminder $[«$comment»] $[:target: for $[$targetName ## you]] on $date at $time";
    pure_parser_enable_alias(&parser, "target");
    pure_parser_assign_var(&parser, "comment", "Check his payment");
    pure_parser_assign_var(&parser, "date", "today");
    pure_parser_assign_var(&parser, "time", "11:30 AM");
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_enable_alias(meta, 0, "target");
    example_meta_set_variable(meta, 0, "comment", "Check his payment");
    example_meta_set_variable(meta, 1, "date", "today");
    example_meta_set_variable(meta, 2, "time", "11:30 AM");
    example_meta_set_reference(meta, "You changed reminder «Check his payment» for you on today at 11:30 AM");
}

static void test_ComplexInactiveAlias(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "$[Agent $creatorName ## You] changed reminder $[«$comment»] $[:target: for $[$targetName ## you]] on $date at $time";
    pure_parser_assign_var(&parser, "comment", "Check his payment");
    pure_parser_assign_var(&parser, "date", "today");
    pure_parser_assign_var(&parser, "time", "11:30 AM");
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_reset_aliases(meta);
    example_meta_set_variable(meta, 0, "comment", "Check his payment");
    example_meta_set_variable(meta, 1, "date", "today");
    example_meta_set_variable(meta, 2, "time", "11:30 AM");
    example_meta_set_reference(meta, "You changed reminder «Check his payment» on today at 11:30 AM");
}

static void test_Coupons(example_meta_t *meta) {
    pure_config_t config;
    pure_config_set_default(&config);
    
    pure_parser_t parser;
    pure_parser_init(&parser, &config);
    
    const char *formula = "You have $[:none: no coupons ## :one: one coupon ## $number coupons] $[:expiring: expiring on $date]";
    pure_parser_enable_alias(&parser, "one");
    pure_parser_enable_alias(&parser, "expiring");
    pure_parser_assign_var(&parser, "date", "11/11/19");
    pure_parser_execute(&parser, formula, true, true, &meta->output, NULL);
    
    example_meta_set_formula(meta, formula);
    example_meta_enable_alias(meta, 0, "one");
    example_meta_enable_alias(meta, 1, "expiring");
    example_meta_set_variable(meta, 0, "date", "11/11/19");
    example_meta_set_reference(meta, "You have one coupon expiring on 11/11/19");
}

#pragma mark - Execute all examples

#ifndef main_c
#define main_c main
#endif

int main_c() {
    #define declare_example_case(func) example_make(#func, func)
    const example_t all_examples[] = {
        declare_example_case(test_Plain),
        declare_example_case(test_Variable),
        declare_example_case(test_VariableAndBlockWithVariable),
        declare_example_case(test_VariableAndBlockWithPlain),
        declare_example_case(test_BlockWithRich),
        declare_example_case(test_InactiveBlock),
        declare_example_case(test_ComplexActiveAlias),
        declare_example_case(test_ComplexInactiveAlias),
        declare_example_case(test_Coupons)
    };
    #undef declare_example_case
    
    const size_t number_of_examples = sizeof(all_examples) / sizeof(*all_examples);
    bool passed = true;
    
    for (const example_t *example = all_examples, *end = &all_examples[number_of_examples]; example < end; example++) {
        example_meta_t result;
        example->executor(&result);
        
        printf("Example \"%s\"\n", example->caption);
        printf("> Formula: \"%s\"\n", result.formula);
        
        for (char *variable = (char *) result.variables; *variable;) {
            printf("> Assign variable \"%s\" = ", variable);
            variable += sizeof(char) * 100;

            printf("\"%s\"\n", variable);
            variable += sizeof(char) * 100;
        }

        for (char *alias = (char *) result.aliases; *alias;) {
            printf("> Enable alias \"%s\"\n", alias);
            alias += sizeof(char) * 100;
        }

        printf("> Output: \"%s\"\n", result.output);
        
        if (strcmp(result.output, result.reference) == 0) {
            printf("[OK]\n");
        }
        else {
            printf("> Reference: \"%s\"\n", result.reference);
            printf("[Failed]\n");
            passed = false;
        }
        
        printf("\n");
    }
    
    if (passed) {
        printf("== All examples passed OK ==\n");
        return 0;
    }
    else {
        return 1;
    }
}
