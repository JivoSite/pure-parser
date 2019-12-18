//
// Created by Stan Potemkin on 30.11.2019.
//

#include "PureParser.hpp"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>

#pragma mark - Local Types

typedef struct {
    std::string formula;
    std::map<std::string, std::string> variables;
    std::set<std::string> aliases;
    std::string reference;
    std::string output;
} example_meta_t;

typedef example_meta_t(*example_func_t)(void);

typedef struct {
    std::string caption;
    example_func_t executor;
} example_t;

#pragma mark - Examples

static example_meta_t test_Plain() {
    PureParser parser;
    const std::string formula = "Hello world";

    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "Hello world";
    
    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>(),
        .aliases = std::set<std::string>(),
        .reference = reference,
        .output = output
    };
}

static example_meta_t test_Variable() {
    PureParser parser;
    const std::string formula = "Hello, $name";
    
    parser.assignVariable("name", "Stan");

    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "Hello, Stan";
    
    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>{ {"name", "Stan"} },
        .aliases = std::set<std::string>(),
        .reference = reference,
        .output = output
    };
}

static example_meta_t test_VariableAndBlockWithVariable() {
    PureParser parser;
    const std::string formula = "Please wait, $name: we're calling $[$anotherName ## another guy]";
    
    parser.assignVariable("name", "Stan");
    parser.assignVariable("anotherName", "Paul");
    
    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "Please wait, Stan: we're calling Paul";
    
    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>{ {"name", "Stan"}, {"anotherName", "Paul"} },
        .aliases = std::set<std::string>(),
        .reference = reference,
        .output = output
    };
}

static example_meta_t test_VariableAndBlockWithPlain() {
    PureParser parser;
    const std::string formula = "Please wait, $name: we're calling $[$anotherName ## another guy]";
    
    parser.assignVariable("name", "Stan");
    
    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "Please wait, Stan: we're calling another guy";
    
    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>{ {"name", "Stan"} },
        .aliases = std::set<std::string>(),
        .reference = reference,
        .output = output
    };
}

static example_meta_t test_BlockWithRich() {
    PureParser parser;
    const std::string formula = "Congrats! You saved it $[in folder '$folder'].";
    
    parser.assignVariable("folder", "Documents");
    
    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "Congrats! You saved it in folder 'Documents'.";
    
    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>{ {"folder", "Documents"} },
        .aliases = std::set<std::string>(),
        .reference = reference,
        .output = output
    };
}

static example_meta_t test_InactiveBlock() {
    PureParser parser;
    const std::string formula = "Congrats! You saved it $[in folder '$folder'].";
    
    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "Congrats! You saved it.";
    
    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>(),
        .aliases = std::set<std::string>(),
        .reference = reference,
        .output = output
    };
}
    
static example_meta_t test_ComplexActiveAlias() {
    PureParser parser;
    const std::string formula = "$[Agent $creatorName ## You] changed reminder $[«$comment»] $[:target: for $[$targetName ## you]] on $date at $time";
    
    parser.assignVariable("comment", "Check his payment");
    parser.assignVariable("date", "today");
    parser.assignVariable("time", "11:30 AM");
    parser.enableAlias("target");

    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "You changed reminder «Check his payment» for you on today at 11:30 AM";
    
    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>{ {"comment", "Check his payment"}, {"date", "today"}, {"time", "11:30 AM"} },
        .aliases = std::set<std::string>{ "target" },
        .reference = reference,
        .output = output
    };
}

static example_meta_t test_ComplexInactiveAlias() {
    PureParser parser;
    const std::string formula = "$[Agent $creatorName ## You] changed reminder $[«$comment»] $[:target: for $[$targetName ## you]] on $date at $time";

    parser.assignVariable("comment", "Check his payment");
    parser.assignVariable("date", "today");
    parser.assignVariable("time", "11:30 AM");

    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "You changed reminder «Check his payment» on today at 11:30 AM";

    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>{ {"comment", "Check his payment"}, {"date", "today"}, {"time", "11:30 AM"} },
        .aliases = std::set<std::string>(),
        .reference = reference,
        .output = output
    };
}

static example_meta_t test_Coupons() {
    PureParser parser;
    const std::string formula = "You have $[:none: no coupons ## :one: one coupon ## $number coupons] $[:expiring: expiring on $date]";
    
    parser.enableAlias("one");
    parser.enableAlias("expiring");
    parser.assignVariable("date", "11/11/19");

    const std::string output = parser.execute(formula, true, true);
    const std::string reference = "You have one coupon expiring on 11/11/19";

    return example_meta_t {
        .formula = formula,
        .variables = std::map<std::string, std::string>{ {"date", "11/11/19"} },
        .aliases = std::set<std::string>{ "one", "expiring" },
        .reference = reference,
        .output = output
    };
}

#pragma mark - Execute all examples

#ifndef main_cpp
#define main_cpp main
#endif

int main_cpp() {
    #define declare_example_case(func) example_t{.caption=#func,.executor=func}
    const std::vector<example_t> all_examples {
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

    bool passed = true;
    for (auto &example : all_examples) {
        const example_meta_t meta = example.executor();
        std::cout << "Example \"" << example.caption <<  "\"" << std::endl;
        std::cout << "> Formula: \"" << meta.formula << "\"" << std::endl;
        
        for (const auto &variable : meta.variables) {
            std::cout << "> Assign variable \"" << variable.first << "\" = \"" << variable.second << "\"" << std::endl;
        }

        for (const auto &alias : meta.aliases) {
            std::cout << "> Enable alias \"" << alias << "\"" << std::endl;
        }

        std::cout << "> Output: \"" << meta.output << "\"" << std::endl;
        
        if (meta.output == meta.reference) {
            std::cout << "[OK]" << std::endl;
        }
        else {
            std::cout << "> Reference: \"" << meta.reference << "\"" << std::endl;
            std::cout << "[Failed]" << std::endl;
            passed = false;
        }
        
        std::cout << std::endl;
    }

    if (passed) {
        std::cout << "== All examples passed OK ==" << std::endl;
        return 0;
    }
    else {
        return 1;
    }
}
