#ifndef MAIN_LIB_H
#define MAIN_LIB_H

#include <string>
#include <vector>

// Define DLLEXPORT for Windows, leave as empty for other OS's
#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport)
    #define helper __stdcall
#else
    #define DLLEXPORT
    #define helper
#endif

// Function pointer type for library functions
typedef std::string (*FunctionPtr)(const std::vector<std::string>& args);

// Function declarations with DLLEXPORT
extern "C" DLLEXPORT std::string helper print(const std::vector<std::string>& args);
extern "C" DLLEXPORT std::string helper add(const std::vector<std::string>& args);

// Function to list all available functions in the library
extern "C" DLLEXPORT std::vector<std::string> helper listFunctions();

// Function to get a function pointer by name
extern "C" DLLEXPORT FunctionPtr helper getFunction(const char* name);

#endif // MAIN_LIB_H