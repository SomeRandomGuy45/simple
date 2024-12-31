#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <variant>
#include <cstdlib>
#include <ctime>

#define open extern "C"
#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport)
    #define helper __stdcall
#else
    #define DLLEXPORT __attribute__((visibility("default")))
    #define helper
#endif

using ReturnType = std::variant<std::nullptr_t, std::string>;

// Function pointer type for library functions
typedef ReturnType(*FunctionPtr)(const std::vector<std::string>& args);

// Function to list all available functions in the library
open DLLEXPORT std::vector<std::string> helper listFunctions();

// Function to get a function pointer by name
open DLLEXPORT FunctionPtr helper getFunction(const char* name);

#endif // RANDOM_H