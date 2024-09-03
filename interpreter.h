#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <map>
#include <vector>
#include <functional>

typedef void (*FunctionPtr)(const std::vector<std::string>&); // Function pointer type for functions with arguments

class Interpreter {
public:
    Interpreter();
    ~Interpreter();

    void executeFromFile(const std::string& filename);
    void defineVariable(const std::string& name, const std::string& value);
    std::string GetVarible(const std::string& name);
    void callFunction(const std::string& name, const std::vector<std::string>& args); // Method to call functions with arguments
    void handleFunctionDefinition(const std::string& name, const std::vector<std::string>& body);
    void handleFunctionCall(const std::string& name, const std::vector<std::string>& args); // Updated to handle arguments
    void handleReturn();
    std::vector<std::string> parseArguments(const std::string& argsStr);

private:
    void parseLine(const std::string& line);
    void loadLibrary(const std::string& libName);
    void unloadLibrary(const std::string& libName);
    void* loadLibraryImpl(const std::string& libName);
    void unloadLibraryImpl(void* libHandle);
    void* getFunctionAddress(void* libHandle, const std::string& funcName);

    // Member variables
    std::map<std::string, std::string> variables;
    std::map<std::string, std::function<void(const std::vector<std::string>&)>> functions; // Map updated to handle functions with arguments
    std::map<std::string, void*> loadedLibraries;
};

#endif // INTERPRETER_H