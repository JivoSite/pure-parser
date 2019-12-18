//
//  PureScanner.cpp
//  PureParser
//
//  Copyright © 2019 JivoSite Inc. All rights reserved.
//  <For detailed info about how this parser works, please refer to README.md file>
//

#include "PureScanner.hpp"
#include <string>
#include <optional>

PureScanner::PureScanner(std::string input) {
	reset(input);
}

void PureScanner::reset(std::string input, std::string last_slice) {
    // Save the most input parameters as is
    this->input = input;
	this->last_slice = last_slice;
	this->following_slice = input;
    
    // And extract the iterator over the `input` for future use
	this->_current_iter = this->input.begin();
}

bool PureScanner::canContinue() const {
    // Just true if the iterator did not reach the end of `input`
	return (_current_iter < input.end());
}

bool PureScanner::detectAndSlice(std::string needle) {
    const size_t offset = _current_iter - input.begin();
    if (input.find(needle, offset) == offset) {
        // If `needle` exists at current pointer location,
        // then replace this scanner with a new one
        
        // The contents between previously captured, and until the `needle`,
        // will be saved into `last_slice`
        const std::string found_data = input.substr(0, offset);
        
        // The contents since the `needle` and up to the end,
        // will be used as new `input`
        const std::string next_input = input.substr(offset + needle.length());
        
        reset(next_input, found_data);
	    return true;
    }
    else {
        // If nothing found, make sure
        // to discard the last captured element
    	this->last_slice = std::string();
        
	    return false;
    }
}

bool PureScanner::detectAndExtract(std::string opener_token, std::string closer_token) {
	const size_t base_index = _current_iter - input.begin();
	size_t since_index = base_index;
	size_t depth = 0;

    // Find and extract the payload between tokens;
    // There are some descriptive steps inside
	for (size_t index = since_index, len = input.length(); index < len; index++) {
        // The `closer_token` found
		if (input.find(closer_token, index) == index) {
            // (0) Found the `closer_token` being at root level;
            // this is not the correct behaviour, so abandon this operation
			if (depth == 0) {
				this->last_slice = std::string();
				return false;
			}
            // (2) Found the `closer_token` right before getting back to the root level;
            // this is the end of slice, so capture the contents and use it
			else if (--depth == 0) {
                // The contents between found tokens, and including tokens itselves,
                // will be saved into `last_slice`
			    const std::string found_data = input.substr(since_index, index - since_index);
                
                // The contents since the `closer_token` and up to the end,
                // will be used as new `input`
			    const std::string next_data = input.substr(index + closer_token.length());
                
			    reset(next_data, found_data);
				return true;
			}
		}
        // The `opener_token` found
		else if (input.find(opener_token, index) == index) {
            // (1) Found the `opener_token` being at root level;
            // so, remember the opening position
			if (depth == 0) {
				since_index = index + opener_token.length();
			}

            // And, go deeper
			depth++;
		}
	}

    // If nothing was found, discard the `last_slice`
	this->last_slice = std::string();
    
	return false;
}

bool PureScanner::detectWithCallback(bool(*callback)(char)) {
    return callback(*_current_iter);
}

bool PureScanner::detectWithCallback(bool(*callback)(std::string)) {
    const size_t index = _current_iter - input.begin();
    const std::string slice = input.substr(index);
    return callback(slice);
}

void PureScanner::lookBy(size_t offset) {
    /// Just move the cursor, without the contents modification
    _current_iter += offset;
}

void PureScanner::skipBy(size_t offset) {
    // Caclulate the exact index where to separate the contents
    const size_t index = _current_iter - this->input.begin() + offset;
    
    // The contents until this index, will be saved into the `last_slice`
    const std::string found_data = this->input.substr(0, index);
    
    // The contents since this index and up to the end, will be saved into the `next_data`
    const std::string next_data = this->input.substr(index);
    
    reset(next_data, found_data);
}
