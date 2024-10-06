#include <string>
#include <iostream>

class NodeItem {
public:
    NodeItem() = default;

    // Constructor with name and value
    NodeItem(std::string n, std::string v) : name(std::move(n)), value(std::move(v)) {}

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

private:
    std::string name;
    std::string value;
};