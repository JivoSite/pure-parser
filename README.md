## What is PureParser

[![Platform](https://img.shields.io/cocoapods/p/PureParser.svg?style=flat)](https://cocoapods.org/pods/PureParser)
[![Build Status](https://travis-ci.com/JivoSite/pure-parser.svg?branch=master)](https://travis-ci.com/JivoSite/pure-parser)
[![Version](https://img.shields.io/cocoapods/v/PureParser.svg?style=flat)](https://cocoapods.org/pods/PureParser)
[![License](https://img.shields.io/cocoapods/l/PureParser.svg?style=flat)](https://cocoapods.org/pods/PureParser)

PureParser is a tool that was designed to simplify the process of adopting the localizable strings if many conditions could cause those strings to look in different ways.

PureParser allows putting the basic logic into strings themselves to control the output, and still keep it human-readable.

## Install

### C++ static library

```
git clone https://github.com/JivoSite/pure-parser.git
cd pure-parser
make cpp_lib
```

After that, please check the `build` folder for `libPureParser.a`, `PureParser.hpp`, and `PureElement.hpp` files

### C static library

```
git clone https://github.com/JivoSite/pure-parser.git
cd pure-parser
make c_lib
```

After that, please check the `build` folder for `libpureparser.a` and `pure_parser.h` files

### Swift Package Manager

```
https://github.com/JivoSite/pure-parser.git
```

### Cocoapods

```
pod 'PureParser'
```

## Usage

### C++ example

```
PureParser parser;
const std::string formula = "$[$name has ## You have] $[$number coupon(s) ## no coupons] expiring on $date";

parser.assignVariable("number", "7");
parser.assignVariable("date", "11/11/19");
const std::string result = parser.execute(formula, true, true);

std::cout << result << std::endl;
```
> You have one coupon expiring on 11/11/19

You can find more examples at `./cpp_src/PureParserExamples.cpp`  
and run them by `make cpp_run`

### C example

```
pure_config_t config;
pure_config_set_default(&config);

pure_parser_t parser;
pure_parser_init(&parser, &config);

const char *formula = "$[$name has ## You have] $[$number coupon(s) ## no coupons] expiring on $date";
pure_parser_assign_var(&parser, "number", "7");
pure_parser_assign_var(&parser, "date", "11/11/19");

char *result_buff = NULL;
size_t result_len = 0;
pure_parser_execute(&parser, formula, true, true, &result_buff, &result_len);

if (result_buff) {
    printf("%s\n", result_buff);
    free(result_buff);
}
```
> You have one coupon expiring on 11/11/19

You can find more examples at `./c_wrapper/pure_parser_examples.c`  
and run them by `make c_run`

### Swift example

```
let parser = PureParser()
let formula = "$[$name has ## You have] $[$number coupon(s) ## no coupons] expiring on $date"

parser.assign(variable: "number", value: "7")
parser.assign(variable: "date", value: "11/11/19")
let result = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)

print(result)
```
> You have one coupon expiring on 11/11/19

You can find more examples at `./swift_wrapper/PureParserExamples.swift`  
and run them by `make swift_run`

## What's inside

There are five main terms: **frame**, **variable**, **block**, **alias**, and **formula**.  
Let's take a look a bit closer.

#### Frame

> Frame is a portion of text to be parsed that can contain any logical elements, if needed.

```
PureParser parser;
const char *formula = "Hello world";

parser.execute(formula, true, true);
// "Hello world"
```

#### Variable

> Variable is the named logical element that you could assigned to a textual value, or don't set at all.
>
> By default, variable starts with dollar sign like `$variableName`.

```
PureParser parser;
const char *formula = "You saved the photo into folder '$folder'.";

parser.assignVariable("folder", "Family");
parser.execute(formula, true, true);
// "You saved the photo into folder 'Family'."
```

#### Block

> Block is a logical element that contain one or more **frames** separated by special token. The purpose of **block** is to parse the sequence of its **frames** and get the first **frame** that was parsed successfully.
>
> If none of its frames can be successfully parsed, the block becomes illegal itself and will be skipped by his parent. Blocks can be nested.
>
> By default, block starts with dollar sign, and surrounds his frames between separators by square brackets like `$[first frame ## second frame ## third frame]`.

```
PureParser parser;
const char *formula = "You saved the photo into $[folder '$folder' ## default folder].";

parser.execute(formula, true, true);
// "You saved the photo into default folder."

parser.assignVariable("folder", "Family");
parser.execute(formula, true, true);
// "You saved the photo into folder 'Family'."
```

```
PureParser parser;
const char *formula = "You saved the photo $[into folder '$folder'].";

parser.execute(formula, true, true);
// "You saved the photo."

parser.assignVariable("folder", "Family");
parser.execute(formula, true, true);
// "You saved the photo into folder 'Family'."
```

#### Alias

> Alias is a virtual name for any frame. If you assign alias to the frame, it'll become inactive by default. It means, the parsing process will not use that frame as legal result when executing the block.
> 
> This can be useful if you need to select a frame within block, if your frames does not contain any **variable** to control the parsing flow.
>
> By default, **alias** is surrounded by colons and is placed as first element in the frame like `You have $[:no-money: no ## enough] money`. Aliases are not parsed into output.

```
PureParser parser;
const char *formula = "You have $[:none: no coupons ## :one: one coupon ## $number coupons]";

parser.execute(formula, true, true);
// "" // no alias activated, no variables set

parser.enableAlias("none");
parser.execute(formula, true, true);
// "You have no coupons"

parser.enableAlias("one");
parser.execute(formula, true, true);
// "You have one coupon"

parser.assignVariable("number", "12");
parser.execute(formula, true, true);
// "You have 12 coupons"
```

#### Formula

> Finally, **formula** is just a root frame that you pass into the parser.

## Author and License

JivoSite Inc. <info@jivochat.com>, 2019.

PureParser is available under the MIT license.  
See the LICENSE file for more info.
