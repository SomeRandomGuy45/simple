#include "table/Node.h"
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <variant>

void Node::operator=(Node& other)
{
    children = other.children;
}

bool Node::operator==(Node& other)
{
    return children == other.children;
}

std::string& Node::operator[] (const std::string& name) {
    for (auto& child : children) {
        if (child.getName() == name) {
            return child.getValue(); // Return reference to the value
        }
    }
    // If the child doesn't exist, create it with an empty value
    children.emplace_back(name, "");
    return children.back().getValue();
}

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