#include "main_lib.h"
#include <iostream>
#include <string>
#include <fstream>

std::string removeQuotes(const std::string& str) {
    std::string result = str;

    // Check if the first character is a quote
    if (!result.empty() && result.front() == '"') {
        result.erase(0, 1); // Remove the first quote
    }

    // Check if the last character is a quote
    if (!result.empty() && result.back() == '"') {
        result.pop_back(); // Remove the last quote
    }

    return result;
}

std::string trimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string trimRight(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string trim(const std::string& str) {
    return trimRight(trimLeft(str));
}

std::string print(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        std::cout << removeQuotes(arg) << " ";
    }
    std::cout << std::endl;
    return "";
}

std::string newFile(const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        return "To many args only need 2";
    }
    std::ofstream fill(removeQuotes(args[0]));
    fill << removeQuotes(trim(args[1]));
    fill.close();
    return "Success";
}

std::string add(const std::vector<std::string>& args) {
    if (args.size() > 1) {
        int a = std::stoi(args[0]);
        int b = std::stoi(args[1]);
        int result = a + b;
        return std::to_string(result);
    }
    return "InvalidArgs";
}

std::vector<std::string> listFunctions() {
    return {"print", "add", "newFile"};
}

FunctionPtr getFunction(const char* name) {
    if (std::string(name) == "print") {
        return &print;
    } else if (std::string(name) == "add") {
        return &add;
    } else if (std::string(name) == "newFile")
    {
        return &newFile;
    }
    return nullptr;
}