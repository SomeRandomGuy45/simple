#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <map>
#include <vector>

typedef std::string (*FunctionPtr)(const std::vector<std::string>& args);

class Interpreter {
public:
    Interpreter();
    ~Interpreter();

    // Execute script from file
    void executeFromFile(const std::string& filename);

    // Define a new variable
    void defineVariable(const std::string& name, const std::string& value);

    // Call a function
    std::string callFunction(const std::string& name, const std::vector<std::string>& args);

private:
    // Map of function names to function pointers
    std::map<std::string, FunctionPtr> functions;

    // Map of variable names to their values
    std::map<std::string, std::string> variables;

    // Map of library names to their handles
    std::map<std::string, void*> loadedLibraries;

    // Parse a line of the script
    void parseLine(const std::string& line);

    // Load a dynamic library and register its functions
    void loadLibrary(const std::string& libName);

    // Unload a dynamic library
    void unloadLibrary(const std::string& libName);

    // Get the value of a variable
    std::string GetVarible(const std::string& name);

    // Parse the arguments string into a vector of arguments
    std::vector<std::string> parseArguments(const std::string& argsStr);

    // Utility functions for managing libraries and function addresses
    void* loadLibraryImpl(const std::string& libName);
    void unloadLibraryImpl(void* libHandle);
    void* getFunctionAddress(void* libHandle, const std::string& funcName);
};

#endif // INTERPRETER_H