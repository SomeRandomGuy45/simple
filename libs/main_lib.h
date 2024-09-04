#ifndef MAIN_LIB_H
#define MAIN_LIB_H

#include <string>
#include <vector>

// Function pointer type for library functions
typedef std::string (*FunctionPtr)(const std::vector<std::string>& args);

// Function declarations
extern "C" {
    std::string print(const std::vector<std::string>& args);
    std::string newFile(const std::vector<std::string>& args);
    std::string add(const std::vector<std::string>& args);

    // Function to list all available functions in the library
    std::vector<std::string> listFunctions();

    // Function to get a function pointer by name
    FunctionPtr getFunction(const char* name);
};
#endif // MAIN_LIB_H