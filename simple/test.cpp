#include <iostream>
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>
#include <variant>

using ReturnType = std::variant<std::nullptr_t, std::string>;
using FuncType = std::function<ReturnType(std::vector<std::string>)>;

// Sample function
ReturnType print(std::vector<std::string> args) {
    for (const auto& val : args) {
        std::cout << val << " ";
    }
    std::cout << "\n";
    return nullptr;
}

// Function to return all function names
std::unordered_map<std::string, FuncType> returnAllFuncNamePtr() {
    return {
        {"print", print}
    };
}