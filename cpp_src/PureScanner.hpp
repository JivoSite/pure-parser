//
//  PureScanner.hpp
//  PureParser
//
//  Copyright © 2019 JivoSite Inc. All rights reserved.
//  <For detailed info about how this parser works, please refer to README.md file>
//

#ifndef PureScanner_hpp
#define PureScanner_hpp

#include <string>
#include <optional>

/**
 * The scanner tool,
 * helps doing detection inside an input
 * and extract contents between the keyword tokens
 */
class PureScanner {
public:
    /**
     * Create the scanner using an `input` for operating with
     * @param input an input for operating with
     */
    explicit PureScanner(std::string input);

    /**
     * Configure the scanner from scratch for operating with new `input`,
     * and store the `last_slice` for following purposes
     * @param input a new input for operating with
     * @param last_slice slice for following purposes
     */
    void reset(std::string input, std::string last_slice = std::string());

    /**
     * Detects if the scanner still has data to continue scanning
     * @return whether we can continue the scanning
     */
    bool canContinue() const;

    /**
     * Detects if the current scanning position points to `needle`;
     * if so, the slice between the last detecting and the `needle` are stored into `last_slice`,
     * and the slice following the `needle` are stored into `following_slice`
     * @param needle token used for finding and slicing
     * @return status of operation
     */
    bool detectAndSlice(std::string needle);

    /**
     * Detects if the current scanning position points to subject
     * surrounded by `opener_token` and `closer_token`;
     * if so, the contents including these tokens are stored into `last_slice`,
     * and the contents following the `closer_token` are stored into `following_slice`
     * @param opener_token token that starts the subject
     * @param closer_token token that finishes the subject
     * @return status of operation
     */
    bool detectAndExtract(std::string opener_token, std::string closer_token);
    
    /**
     * Detects if the current scanning position points to subject
     * that could be procesed by callback with success
     * @param callback callback to process the current subject
     * @return status of processing
     */
    bool detectWithCallback(bool(*callback)(char));
    bool detectWithCallback(bool(*callback)(std::string));

    /**
     * Move the internal cursor forward inside the contents
     * @param offset the moving distance
     */
    void lookBy(size_t offset);

    /**
     * Skip some contents
     * @param offset the amount to skip
     */
    void skipBy(size_t offset);

public:
    /// The current input this scanner operates over
    std::string input;

    /// The found slice after recently detecting operation
    std::string last_slice;

    /// The remaining slice after recently detecting operation
    std::string following_slice;

private:
    std::string::iterator _current_iter;
};

#endif /* PureScanner_hpp */
