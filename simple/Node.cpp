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