// swift-tools-version:5.1

import PackageDescription

let package = Package(
    name: "PureParser",
    products: [
        .library(
            name: "PureParser",
            targets: ["PureParserCpp", "PureParserC", "PureParser"]),
        .executable(
            name: "PureParserExamples",
            targets: ["PureParserExamples"])
    ],
    dependencies: [
    ],
    targets: [
        .target(
            name: "PureParserCpp",
            dependencies: [],
            path: "cpp_src",
            sources: [
                "PureScanner.cpp", "PureParser.cpp"
            ],
            publicHeadersPath: "."),
        .target(
            name: "PureParserC",
            dependencies: ["PureParserCpp"],
            path: "c_wrapper",
            sources: [
                "pure_parser.cpp"
            ],
            publicHeadersPath: "."),
        .target(
            name: "PureParser",
            dependencies: ["PureParserC"],
            path: "swift_wrapper",
            sources: [
                "PureParser.swift"
            ]),
        .target(
            name: "PureParserExamples",
            dependencies: ["PureParser"],
            path: "swift_wrapper",
            sources: [
                "PureParserExamples.swift",
                "main.swift"
            ],
            swiftSettings: [
                .define("OUTER_EXECUTION")
            ])
    ],
    cxxLanguageStandard: .cxx1z
)
