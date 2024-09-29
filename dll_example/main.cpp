#include "main_lib.h"
#include <iostream>

extern "C" DLLEXPORT std::string helper print(const std::vector<std::string>& args) {
    if (args.size() > 0) {
        std::cout << args[0] << std::endl;
        return "Printed";
    }
    return "NoArgs";
}

extern "C" DLLEXPORT std::string helper add(const std::vector<std::string>& args) {
    if (args.size() > 1) {
        int a = std::stoi(args[0]);
        int b = std::stoi(args[1]);
        int result = a + b;
        return std::to_string(result);
    }
    return "InvalidArgs";
}

extern "C" DLLEXPORT std::vector<std::string> helper listFunctions() {
    return {std::string("print"), std::string("add")};
}

extern "C" DLLEXPORT FunctionPtr helper getFunction(const char* name) {
    if (std::string(name) == "print") {
        return &print;
    } else if (std::string(name) == "add") {
        return &add;
    }
    return nullptr;
}
