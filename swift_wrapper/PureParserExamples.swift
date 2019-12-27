fileprivate struct Meta {
    let formula: String
    let variables: [String: String]
    let aliases: [String]
    let output: String
    let reference: String
    
    var hasPassed: Bool {
        return (output == reference)
    }
}

fileprivate func execute_Plain() -> Meta {
    let parser = PureParser()
    let formula = "Hello world"
    
    let output = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)
    
    return Meta(
        formula: formula,
        variables: [:],
        aliases: [],
        output: output,
        reference: "Hello world"
    )
}

fileprivate func execute_Variable() -> Meta {
    let parser = PureParser()
    let formula = "Hello, $name"
    
    parser.assign(variable: "name", value: "Stan")
    let output = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)

    return Meta(
        formula: formula,
        variables: ["name": "Stan"],
        aliases: [],
        output: output,
        reference: "Hello, Stan"
    )
}

fileprivate func execute_VariableAndBlockWithVariable() -> Meta {
    let parser = PureParser()
    let formula = "Please wait, $name: we're calling $[$anotherName ## another guy]"
    
    parser.assign(variable: "name", value: "Stan")
    parser.assign(variable: "anotherName", value: "Paul")
    let output = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)

    return Meta(
        formula: formula,
        variables: ["name": "Stan", "anotherName": "Paul"],
        aliases: [],
        output: output,
        reference: "Please wait, Stan: we're calling Paul"
    )
}

fileprivate func execute_VariableAndBlockWithPlain() -> Meta {
    let parser = PureParser()
    let formula = "Please wait, $name: we're calling $[$anotherName ## another guy]"
    
    parser.assign(variable: "name", value: "Stan")
    let output = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)

    return Meta(
        formula: formula,
        variables: ["name": "Stan"],
        aliases: [],
        output: output,
        reference: "Please wait, Stan: we're calling another guy"
    )
}

fileprivate func execute_BlockWithRich() -> Meta {
    let parser = PureParser()
    let formula = "Congrats! You saved it $[in folder '$folder']."
    
    parser.assign(variable: "folder", value: "Documents")
    let output = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)

    return Meta(
        formula: formula,
        variables: ["folder": "Documents"],
        aliases: [],
        output: output,
        reference: "Congrats! You saved it in folder 'Documents'."
    )
}

fileprivate func execute_InactiveBlock() -> Meta {
    let parser = PureParser()
    let formula = "Congrats! You saved it $[in folder '$folder']."
    
    let output = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)
    
    return Meta(
        formula: formula,
        variables: [:],
        aliases: [],
        output: output,
        reference: "Congrats! You saved it."
    )
}

fileprivate func execute_Complex() -> Meta {
    let parser = PureParser()
    let formula = "$[Agent $creatorName ## You] changed reminder $[«$comment»] $[:target: for $[$targetName ## you]] on $date at $time"
    
    parser.assign(variable: "comment", value: "Check his payment")
    parser.assign(variable: "date", value: "today")
    parser.assign(variable: "time", value: "11:30 AM")
    parser.activate(alias: "target", true)
    let output = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)

    return Meta(
        formula: formula,
        variables: ["comment": "Check his payment", "date": "today", "time": "11:30 AM"],
        aliases: ["target"],
        output: output,
        reference: "You changed reminder «Check his payment» for you on today at 11:30 AM"
    )
}

fileprivate func execute_Coupons() -> Meta {
    let parser = PureParser()
    let formula = "$[$name has ## You have] $[$number coupon(s) ## no coupons] expiring on $date"
    
    parser.assign(variable: "number", value: "7")
    parser.assign(variable: "date", value: "11/11/19")
    let output = parser.execute(formula, collapseSpaces: true, resetOnFinish: true)

    return Meta(
        formula: formula,
        variables: ["number": "7", "date": "11/11/19"],
        aliases: [],
        output: output,
        reference: "You have 7 coupon(s) expiring on 11/11/19"
    )
}

fileprivate func deepPrint(name: String, meta: Meta) {
    print("Example \"\(name)\"")
    print("> Formula: \"\(meta.formula)\"")
    meta.variables.forEach { variable in print("> Assign variable \"\(variable.key)\" = \"\(variable.value)\"") }
    meta.aliases.forEach { alias in print("> Enable alias \"\(alias)\"") }
    print("> Output: \"\(meta.output)\"")
    
    if meta.hasPassed {
        print("[OK]")
    }
    else {
        print("> Reference: \"\(meta.reference)\"")
        print("[Failed]")
    }
    
    print("")
}

#if OUTER_EXECUTION
import PureParser

final class PureParserExamples {
    func run() {
        let examples = [
            ("test_Plain", execute_Plain),
            ("test_Variable", execute_Variable),
            ("test_VariableAndBlockWithVariable", execute_VariableAndBlockWithVariable),
            ("test_VariableAndBlockWithPlain", execute_VariableAndBlockWithPlain),
            ("test_BlockWithRich", execute_BlockWithRich),
            ("test_InactiveBlock", execute_InactiveBlock),
            ("test_Complex", execute_Complex),
            ("test_Coupons", execute_Coupons)
        ]
        
        var passed = true
        for (name, example) in examples {
            let meta = example()
            deepPrint(name: name, meta: meta)
            
            if !meta.hasPassed {
                passed = false
            }
        }
        
        if passed {
            print("== All examples passed OK ==")
        }
    }
}
#else
import XCTest

final class PureParserExamples: XCTestCase {
    static var allTests = [
        ("test_Plain", test_Plain),
        ("test_Variable", test_Variable),
        ("test_VariableAndBlockWithVariable", test_VariableAndBlockWithVariable),
        ("test_VariableAndBlockWithPlain", test_VariableAndBlockWithPlain),
        ("test_BlockWithRich", test_BlockWithRich),
        ("test_InactiveBlock", test_InactiveBlock),
        ("test_Complex", test_Complex),
        ("test_Coupons", test_Coupons)
    ]
    
    func test_Plain() {
        let meta = execute_Plain()
        deepPrint(name: #function, meta: meta)
        XCTAssert(meta.hasPassed)
    }

    func test_Variable() {
        let meta = execute_Variable()
        deepPrint(name: #function, meta: meta)
        XCTAssert(meta.hasPassed)
    }

    func test_VariableAndBlockWithVariable() {
        let meta = execute_VariableAndBlockWithVariable()
        deepPrint(name: #function, meta: meta)
        XCTAssert(meta.hasPassed)
    }

    func test_VariableAndBlockWithPlain() {
        let meta = execute_VariableAndBlockWithPlain()
        deepPrint(name: #function, meta: meta)
        XCTAssert(meta.hasPassed)
    }

    func test_BlockWithRich() {
        let meta = execute_BlockWithRich()
        deepPrint(name: #function, meta: meta)
        XCTAssert(meta.hasPassed)
    }

    func test_InactiveBlock() {
        let meta = execute_InactiveBlock()
        deepPrint(name: #function, meta: meta)
        XCTAssert(meta.hasPassed)
    }

    func test_Complex() {
        let meta = execute_Complex()
        deepPrint(name: #function, meta: meta)
        XCTAssert(meta.hasPassed)
    }
    
    func test_Coupons() {
        let meta = execute_Coupons()
        deepPrint(name: #function, meta: meta)
        XCTAssert(meta.hasPassed)
    }
}
#endif
