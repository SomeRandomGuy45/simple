#include "table/Node.h"
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <variant>

std::string removeComments(const std::string& input) {
    std::string result;
    std::istringstream stream(input);
    std::string line;
    while (std::getline(stream, line)) {
        // Ignore everything after "//"
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        result += line + "\n";
    }
    return result;
}

Node parse(std::string input)
{
    Node root;
    std::regex entryPattern(R"(\s*\"([^\"]+)\"\s*=\s*(\"([^\"]+)\"|([a-zA-Z_][a-zA-Z0-9_]*))\s*)");
    std::smatch matches;

    std::string sanitizedInput = removeComments(input);  // Remove comments first

    std::string::const_iterator searchStart(sanitizedInput.cbegin());
    while (std::regex_search(searchStart, sanitizedInput.cend(), matches, entryPattern)) {
        std::string key = matches[1];
        std::string value = matches[3].matched ? matches[3] : matches[4]; // Capture value from either string or variable

        // If the value is a variable, check if it's defined in the table
        if (matches[4].matched) {
            if (var_names.find(value) != var_names.end()) {
                value = root[value]; // Resolve variable to its value
            }
        }

        root[key] = value;
        searchStart = matches.suffix().first;
    }

    return root;
}