#include "main_lib.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;


// Define the functions that accept arguments
void print(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
}

// Map function names to function pointers
std::map<std::string, FunctionPtr> functionMap = {
    {"print", print},
};

extern "C" FunctionPtr getFunction(const char* name) {
    auto it = functionMap.find(name);
    return (it != functionMap.end()) ? it->second : nullptr;
}

extern "C" std::vector<std::string> listFunctions() {
    std::vector<std::string> functionNames;
    for (const auto& pair : functionMap) {
        functionNames.push_back(pair.first);
    }
    return functionNames;
}