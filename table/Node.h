#ifndef NODE_M
#define NODE_M
#include "NodeItem.h"
#include <stdexcept>
#include <vector>
#include <unordered_map>

class Node {
public:
    // Constructors
    Node() = default;
    Node(Node&&) = default;
    Node(const Node&) = default;
    
    //Destructor
    ~Node() = default;

    // Operators
    void operator=(Node& other);
    bool operator==(Node& other);
    std::string& operator[] (const std::string& name);

private:
    std::vector<NodeItem> children;
};

std::string removeComments(const std::string& input);

#endif