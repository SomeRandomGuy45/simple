#include "NodeItem.h"
#include <stdexcept>
#include <vector>
#include <unordered_map>

std::unordered_map<std::string, std::string> var_names;

class Node {
public:

    void operator=(Node& other)
    {
        children = other.children;
    }

    std::string& operator[] (const std::string& name) {
        for (auto& child : children) {
            if (child.getName() == name) {
                return child.getValue(); // Return reference to the value
            }
        }
        // If the child doesn't exist, create it with an empty value
        children.emplace_back(name, "");
        return children.back().getValue();
    }

private:
    std::vector<NodeItem> children;
};

Node parse(std::string data);