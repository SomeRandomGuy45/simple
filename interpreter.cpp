#include "interpreter.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>

#ifdef _WIN32
#include <Windows.h>
#define LIB_EXT ".dll"
#define LOAD_LIB(name) LoadLibraryA(name)
#define UNLOAD_LIB(lib) FreeLibrary((HMODULE)lib)
#define GET_FUNC(lib, func) reinterpret_cast<void*>(GetProcAddress((HMODULE)lib, func))
#define LIBPATH "C:\\Program Files\\simple_libs\\lib"
#else
#include <dlfcn.h>
#define LIBPATH "/usr/local/lib/simple_libs/lib"
#ifdef __APPLE__
#define LIB_EXT ".dylib"
#else
#define LIB_EXT ".so"
#endif
#define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
#define UNLOAD_LIB(lib) dlclose(lib)
#define GET_FUNC(lib, func) dlsym(lib, func)
#endif

// Helper functions for trimming
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

std::string extractLibName(const std::string& path) {
#ifdef _WIN32
    size_t lastSlashPos = path.find_last_of("\\/");
    std::string fileName = path.substr(lastSlashPos + 1);
    size_t dotPos = fileName.find_last_of('.');
    std::string baseName = fileName.substr(0, dotPos);
    if (baseName.compare(0, 3, "lib") == 0) {
        baseName.erase(0, 3);
    }
    return baseName;
#else
    size_t lastSlashPos = path.find_last_of('/');
    std::string fileName = path.substr(lastSlashPos + 1);
    size_t dotPos = fileName.find_last_of('.');
    std::string baseName = fileName.substr(0, dotPos);
    if (baseName.compare(0, 3, "lib") == 0) {
        baseName.erase(0, 3);
    }
    return baseName;
#endif
}

Interpreter::Interpreter() {}

Interpreter::~Interpreter() {
    for (auto& pair : loadedLibraries) {
        unloadLibrary(pair.first);
    }
}

void Interpreter::executeFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        parseLine(line);
    }

    file.close();
}

std::string Interpreter::GetVarible(const std::string& name)
{
    auto it = variables.find(name);
    if (it!= variables.end()) {
        return it->second;
    } else {
        return "";
    }
}

std::vector<std::string> Interpreter::parseArguments(const std::string& argsStr) {
    std::vector<std::string> args;
    std::string trimmedArgsStr = trim(argsStr);
    
    // Remove surrounding parentheses if any
    if (trimmedArgsStr.front() == '(' && trimmedArgsStr.back() == ')') {
        trimmedArgsStr = trimmedArgsStr.substr(1, trimmedArgsStr.size() - 2);
    }

    // Define regex pattern outside any lambda or function
    std::regex argRegex(R"("([^"]*)|[^,]+)");
    auto argsBegin = std::sregex_iterator(trimmedArgsStr.begin(), trimmedArgsStr.end(), argRegex);
    auto argsEnd = std::sregex_iterator();

    for (std::sregex_iterator i = argsBegin; i != argsEnd; ++i) {
        std::smatch match = *i;
        std::string arg = match.str();
        if (!arg.empty() && arg.front() == '"') {
            arg = arg.substr(1, arg.size() - 2); // Remove surrounding quotes
        }
        if (!GetVarible(arg).empty()) {
            arg = GetVarible(arg);
            if (!arg.empty() && arg.front() == '"') {
                arg = arg.substr(1, arg.size() - 2); // Remove surrounding quotes
            }
        }
        args.push_back(arg);
    }

    return args;
}

void Interpreter::parseLine(const std::string& line) {
    if (line.find("add") != std::string::npos) {
        std::string libName = LIBPATH + line.substr(4) + LIB_EXT;
        loadLibrary(libName);
    } else if (line.find("local") != std::string::npos) {
        std::regex regex(R"(local\s+(\w+)\s*=\s*(.+))");
        std::smatch match;
        if (std::regex_match(line, match, regex)) {
            std::string name = match[1];
            std::string value = match[2];
            defineVariable(name, value);
        }
    } else if (line.find("func") != std::string::npos) {
        std::string name = line.substr(5, line.find('(') - 5);
        std::vector<std::string> body;
        body.push_back(line.substr(line.find(')') + 1));
        handleFunctionDefinition(name, body);
    } else if (line.find("return") != std::string::npos) {
        handleReturn();
    } else if (std::regex_search(line, std::regex(R"(\w+\.\w+\s*\(.*\))"))) {
        std::regex regex(R"((\w+)\.(\w+)\s*\((.*)\))");
        std::smatch match;
        std::string newline = trim(line);
        
        if (std::regex_match(newline, match, regex)) {
            std::string libPrefix = match[1];
            std::string funcName = match[2];
            std::string fullFuncName = libPrefix + "." + funcName;  // Combine to form the full function name
            std::string argsStr = match[3];
            std::vector<std::string> args = parseArguments(argsStr);
            
            callFunction(fullFuncName, args);  // Use the full function name with the prefix
        } else {
            std::cerr << "Error: Failed to match function call in line: " << newline << "\n";
        }
    }
    else if (line.find("//") != std::string::npos)
    {
        // Ignore comments
        return;
    }
}

void Interpreter::defineVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

void Interpreter::callFunction(const std::string& name, const std::vector<std::string>& args) {
    if (functions.find(name) != functions.end()) {
        functions[name](args);
    } else {
        std::cerr << "Function not found: " << name << std::endl;
    }
}

void Interpreter::handleFunctionDefinition(const std::string& name, const std::vector<std::string>& body) {
    functions[name] = [this, body](const std::vector<std::string>& args) {
        // You can now use 'args' to replace arguments in the function body
        for (const auto& line : body) {
            std::string processedLine = line;
            // Example: Replace placeholders with actual arguments
            for (size_t i = 0; i < args.size(); ++i) {
                std::regex argRegex("\\{"+std::to_string(i)+"\\}"); // Example placeholder {0}, {1}, ...
                processedLine = std::regex_replace(processedLine, argRegex, args[i]);
            }
            for (const auto& var : variables) {
                std::regex varRegex("\\b" + var.first + "\\b");
                processedLine = std::regex_replace(processedLine, varRegex, var.second);
            }
            //std::cout << processedLine << std::endl;
        }
    };
}

void Interpreter::handleReturn() {
    //std::cout << "Returning SUCCESS_EXIT" << std::endl;
    exit(0);
}

void Interpreter::loadLibrary(const std::string& libName) {
    void* libHandle = loadLibraryImpl(libName);
    if (!libHandle) {
        std::cerr << "Failed to load library: " << libName << std::endl;
        return;
    }

    std::string call_lib_name = extractLibName(libName) + ".";
    loadedLibraries[libName] = libHandle;

    typedef FunctionPtr (*GetFunctionType)(const char*);
    GetFunctionType getFunction = reinterpret_cast<GetFunctionType>(getFunctionAddress(libHandle, "getFunction"));

    if (getFunction) {
        typedef std::vector<std::string> (*ListFunctionsType)();
        ListFunctionsType listFunctions = reinterpret_cast<ListFunctionsType>(getFunctionAddress(libHandle, "listFunctions"));

        if (listFunctions) {
            std::vector<std::string> functionNames = listFunctions();
            for (const auto& funcName : functionNames) {
                FunctionPtr funcPtr = reinterpret_cast<FunctionPtr>(getFunction(funcName.c_str()));
                if (funcPtr) {
                    functions[call_lib_name + funcName] = funcPtr;
                } else {
                    std::cerr << "Function not found in library: " << funcName << std::endl;
                }
            }
        } else {
            std::cerr << "listFunctions function not found in library: " << libName << std::endl;
        }
    } else {
        std::cerr << "getFunction function not found in library: " << libName << std::endl;
    }
}


void Interpreter::unloadLibrary(const std::string& libName) {
    auto it = loadedLibraries.find(libName);
    if (it != loadedLibraries.end()) {
        unloadLibraryImpl(it->second);
        loadedLibraries.erase(it);
    }
}

void* Interpreter::loadLibraryImpl(const std::string& libName) {
    return LOAD_LIB(libName.c_str());
}

void Interpreter::unloadLibraryImpl(void* libHandle) {
    UNLOAD_LIB(libHandle);
}

void* Interpreter::getFunctionAddress(void* libHandle, const std::string& funcName) {
    return GET_FUNC(libHandle, funcName.c_str());
}