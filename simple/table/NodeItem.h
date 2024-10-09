#include <string>
#include <iostream>

class NodeItem {
public:
    // Constructors
    NodeItem() = default;
    NodeItem(std::string n, std::string v) : name(std::move(n)), value(std::move(v)) {}

    // Destructor
    ~NodeItem() = default;

    // Getter for the name
    std::string getName() const { return name; }

    // Getter for the value
    std::string& getValue() { return value; }

    // Overloaded assignment operator to set the value
    NodeItem& operator=(const std::string& v) {
        value = v;  // Assign the new value
        return *this;
    }

    // Conversion operator to get the value as a string
    operator std::string() const {
        return value;
    }

    // Equality operator
    bool operator==(const NodeItem& other) const {
        return name == other.name && value == other.value;
    }

private:
    std::string name;
    std::string value;
};