cmake_minimum_required(VERSION 3.15)
project(cpp_src)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(cpp_src
    PureElement.hpp
    PureParser.cpp
    PureParser.hpp
    PureParserExamples.cpp
    PureScanner.cpp
    PureScanner.hpp)
