//
//  PureParser.swift
//  PureParser
//
//  Copyright © 2019 JivoSite Inc. All rights reserved.
//  <For detailed info about how this parser works, please refer to README.md file>
//

import Foundation

#if canImport(PureParserC)
import PureParserC
#endif

public protocol IPureParser: class {
    func assign(variable name: String, value: String?)
    func activate(alias: String, _ rule: Bool)
    func execute(_ formula: String, collapseSpaces: Bool, resetOnFinish: Bool) -> String
}

@objc(PureParser) public final class PureParser: NSObject, IPureParser {
    private var config = pure_config_t()
    private var parser = pure_parser_t()
    
    public override init() {
        super.init()
        pure_config_set_default(&config)
        pure_parser_init(&parser, &config)
    }
    
    deinit {
        pure_parser_destroy(&parser)
    }
    
    @objc(assignVariable:value:) public func assign(variable name: String, value: String?) {
        if let value = value {
            pure_parser_assign_var(&parser, name, value)
        }
        else {
            pure_parser_discard_var(&parser, name)
        }
    }
    
    @objc(activateAlias:rule:) public func activate(alias: String, _ rule: Bool) {
        if rule {
            pure_parser_enable_alias(&parser, alias)
        }
        else {
            pure_parser_discard_alias(&parser, alias)
        }
    }
    
    @objc(execute:collapseSpaces:resetOnFinish:) public func execute(_ formula: String, collapseSpaces: Bool, resetOnFinish: Bool) -> String {
        var result: UnsafeMutablePointer<Int8>?
        var result_len = UInt(0)
        pure_parser_execute(&parser, formula, collapseSpaces, resetOnFinish, &result, &result_len)
        
        if let result = result, result_len > 0 {
            return String(bytesNoCopy: result, length: Int(result_len), encoding: .utf8, freeWhenDone: true) ?? String()
        }
        else {
            return String()
        }
    }
}
