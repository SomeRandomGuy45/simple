#ifndef MAIN_LIB_H
#define MAIN_LIB_H

#include <vector>
#include <string>

typedef void (*FunctionPtr)(const std::vector<std::string>&);

extern "C" {
    FunctionPtr getFunction(const char* name);
    std::vector<std::string> listFunctions();
    void print(const std::vector<std::string>& args);
}

#endif // MAIN_LIB_H