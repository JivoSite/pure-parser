//
//  PureElement.hpp
//  PureParser
//
//  Copyright © 2019 JivoSite Inc. All rights reserved.
//  <For detailed info about how this parser works, please refer to README.md file>
//

#ifndef PureElement_hpp
#define PureElement_hpp

#include <string>
#include <list>
#include <optional>
#include <variant>

enum PureElementType {
    /// `payload` is the alias name;
    /// `children` are the frame's elements
    PureElementTypeFrame,
    
    /// `payload` is empty;
    /// `children` are the block's elements
    PureElementTypeBlock,
    
    /// `payload` is the variable name;
    /// `children` are empty
    PureElementTypeVariable,
    
    /// `payload` is the textual content;
    /// `children` are empty
    PureElementTypeSlice
};

struct PureElement {
    PureElementType type;
    std::string payload;
    std::list<PureElement> children;

    PureElement(PureElementType type, std::string payload = std::string(), std::list<PureElement> children = {}) {
        this->type = type;
        this->payload = payload;
        this->children = children;
    }

    PureElement(const PureElement &other) {
        this->type = other.type;
        this->payload = other.payload;
        this->children = other.children;
    }
};

#endif /* PureElement_hpp */
