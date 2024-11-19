#include "base_func.h"
#include <cmath>
#include <iomanip>

/*
* With functions you can return 2 types
* 1. nullptr
* 2. std::string
*/

#ifdef __cplusplus
extern "C" {
#endif

std::unordered_map<std::string, FunctionPtr> outerFunctions;
std::unordered_map<std::string, void*> loadedLibraries;
std::vector<void*> allocatedBlocks;
char** ARG_INPUT;
int ARG_INPUT_LENGTH;

ReturnType print(std::vector<std::string> args)
{
	for (auto& val : args)
	{
        if ((val.find("\\n") != std::string::npos))
        {
            size_t start_pos = 0;
            while ((start_pos = val.find("\\n", start_pos)) != std::string::npos) {
                val.replace(start_pos, std::string("\\n").length(), "\n");
                start_pos += 1;
            }
        }
        else if (val == "\\n")
        {
            val = "\n";
        }
        std::cout << val << " ";
	}
	std::cout << "\n";
    return nullptr;
}

ReturnType add(std::vector<std::string> args)
{
    size_t finalNum = 0;
    for (const auto& val : args)
    {
        try {
            finalNum += std::stoull(val);
        } catch (...) {};
    }
    return std::to_string(finalNum);
}

ReturnType sub(std::vector<std::string> args)
{
    size_t finalNum = 0;
    for (const auto& val : args)
    {
        try {
            finalNum -= std::stoull(val);
        } catch (...) {};
    }
    return std::to_string(finalNum);
}

ReturnType multi(std::vector<std::string> args)
{
    size_t finalNum = 1;
    for (const auto& val : args)
    {
        try {
            finalNum *= std::stoull(val);
        } catch (...) {};
    }
    return std::to_string(finalNum);
}

ReturnType divi(std::vector<std::string> args)
{
    size_t finalNum = 1;
    for (const auto& val : args)
    {
        try {
            finalNum /= std::stoull(val);
        } catch (...) {};
    }
    return std::to_string(finalNum);
}

ReturnType writeData(std::vector<std::string> args)
{
    if (args.size() != 2)
    {
        std::cout << "[WRITE] Error: Invalid number of arguments\n";
        return nullptr;
    }
    std::ofstream file(args[0]);
    file << args[1] << std::endl;
    return nullptr;
}

ReturnType readData(std::vector<std::string> args)
{
    if (args.size() != 1)
    {
        std::cout << "[READ] Error: Invalid number of arguments\n";
        return nullptr;
    }
    std::ifstream file(args[0]);
    std::ostringstream oss;
    oss << file.rdbuf();
    file.close();
    return oss.str();
}

ReturnType isANumber(std::vector<std::string> args)
{
    if (args.size() != 1)
    {
        std::cout << "[ISNUMBER] Error: Invalid number of arguments\n";
        return nullptr;
    }
    const std::string& s = args[0];
    if (!s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end())
    {
        return "true";
    }
    else
    {
        return "false";
    }
    return nullptr;
}

ReturnType runSysCmd(std::vector<std::string> args)
{
    if (args.size() != 1)
    {
        std::cout << "[RUNSYS] Error: Invalid number of arguments\n";
        return nullptr;
    }
    int returnCode = system(args[0].c_str());
    return std::to_string(returnCode);
}

ReturnType sinFunc(std::vector<std::string> args)
{
    if (args.size() != 1)
    {
        std::cout << "[SIN] Error: Invalid number of arguments\n";
        return nullptr;
    }
    double angle;
    try {
        angle = std::stod(args[0]);
    } catch (std::exception& e) {
        std::cerr << "[SIN] Error: Unable to convert to double\n";
    } catch (...) {
        std::cerr << "[SIN] Error: A unknown error has occurred.\n";
    }
    double result = sin(angle);
    return std::to_string(result);
}

ReturnType allocMemory(std::vector<std::string> args)
{
    if (args.size() != 1)
    {
        std::cout << "[ALLOC] Error: Invalid number of arguments\n";
        return nullptr;
    }
    size_t size = 0;
    try {
        size = std::stoull(args[0]);
    } 
    catch (const std::invalid_argument& e)
    {
        std::cout << "[ALLOC] Error: Invalid size argument\n";
        return nullptr;
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "[ALLOC] Error: Size argument out of range\n";
        return nullptr;
    }
    void* newAllocMem = nullptr;
    try {
        newAllocMem = std::malloc(size);
    } catch (...) {
        std::cout << "[ALLOC] Error: A unknown error has occurred. Some issues could be there is not enough ram that can be used to allocate.\n";
        return nullptr;
    }
    if (newAllocMem == nullptr)
    {
        std::cout << "[ALLOC] Error: Failed to allocate memory\n";
        return nullptr;
    }
    allocatedBlocks.push_back(newAllocMem);
    return std::to_string(allocatedBlocks.size() - 1);
}

ReturnType freeMemory(std::vector<std::string> args)
{
    if (args.size() != 1)
    {
        std::cout << "[FREE] Error: Invalid number of arguments\n";
        return nullptr;
    }
    size_t index = 0;
    try {
        index = std::stoull(args[0]);
    } 
    catch (const std::invalid_argument& e)
    {
        std::cout << "[FREE] Error: Invalid index argument\n";
        return nullptr;
    }
    catch (const std::out_of_range& e)
    {
        std::cout << "[FREE] Error: Index argument out of range\n";
        return nullptr;
    }
    if (index < allocatedBlocks.size()) {
        void* toBeFreed = allocatedBlocks[index];
        try {
            std::free(toBeFreed);
            allocatedBlocks[index] = nullptr;
        } catch (const std::exception& e) {
            std::cerr << "[FREE] Error: Failed to free memory: " << e.what() << "\n";
            return nullptr;
        } catch (...) {
            std::cerr << "[FREE] Error: A unknown error has occurred.\n";
            return nullptr;
        }
        return "success";
    }
    std::cout << "[FREE] Out of range!\n";
    return nullptr;
}

ReturnType exportFuncs(std::vector<std::string> args) {
    if (args.size() < 1)
    {
        std::cout << "[EXPORT] Error: Invalid number of arguments\n";
        return nullptr;
    }
    std::cout << "[EXPORT] Hey! This function is still WIP!\n[EXPORT] Once the function/feature is finshed please include !define MODULE_EXPORT true\n[EXPORT] Thank you!\n";
    return nullptr;
}

ReturnType exit_program(std::vector<std::string> args) {
    std::exit(0);
    return nullptr;
}

ReturnType get_users_input(std::vector<std::string> args) {
    size_t count = 0;
    size_t max_size = 1;
    if (args.size() == 1)
    {
        max_size = std::stoll(args[0]);
    }
    std::string input;
    std::string line;
    while (true) {
        if (max_size <= count)
        {
            break;
        }
        std::getline(std::cin, line);

        // Append the new line to the input string
        input += line + "\n";  // Add a newline after each input line
        ++count;
    }
    return input;
}

ReturnType getArgs(std::vector<std::string> args) {
    if (args.size() != 1)
    {
        std::cout << "[GETARGS] Error: Invalid number of arguments\n";
        return nullptr;
    }
    std::string argStr = args[0];
    if (ARG_INPUT[std::stoi(argStr)] != nullptr)
    {
        return std::string(ARG_INPUT[std::stoi(argStr)]);
    }
    return nullptr;
}

//The holder of all the functions
//This looks trash and I really need to find a different way lol
std::unordered_map<std::string, std::function<ReturnType(std::vector<std::string>)>> returnAllFuncName() {
    return {
        {"print", print},
        {"writeToFile", writeData},
        {"readFile", readData},
        {"system", runSysCmd},
        {"sin", sinFunc},
        {"allocMemory", allocMemory},
        {"freeMemory", freeMemory},
        {"add", add},
        {"sub", sub},
        {"div", divi},
        {"multi", multi},
        {"export", exportFuncs},
        {"exit", exit_program},
        {"getInput", get_users_input},
        {"getArgs", getArgs},
        {"isNumber", isANumber},
    };
}

static void* loadLibraryImpl(const std::string& libName) {
    return LOAD_LIB(libName.c_str());
}

void unloadLibraryImpl(void* libHandle) {
    UNLOAD_LIB(libHandle);
}

void* getFunctionAddress(void* libHandle, const std::string& funcName) {
    return GET_FUNC(libHandle, funcName.c_str());
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

void loadLibrary(const std::string& libName) {
    void* libHandle = loadLibraryImpl(libName);
    if (!libHandle) {
        std::cerr << "[VM] Failed to load library: " << libName << std::endl;
        return;
    }

    std::string call_lib_name = extractLibName(libName) + ".";
    loadedLibraries[libName] = libHandle;

    typedef FunctionPtr(*GetFunctionType)(const char*);
    GetFunctionType getFunction = reinterpret_cast<GetFunctionType>(getFunctionAddress(libHandle, "getFunction"));

    if (!getFunction) {
        std::cerr << "[VM] getFunction address is null for library: " << libName << std::endl;
        return;
    }

    typedef std::vector<std::string>(*ListFunctionsType)(); // Default is __cdecl
    ListFunctionsType listFunctions = reinterpret_cast<ListFunctionsType>(getFunctionAddress(libHandle, "listFunctions"));

    if (!listFunctions) {
        std::cerr << "[VM] listFunctions address is null for library: " << libName << std::endl;
        return;
    }

    try {

        for (const std::string& funcName : listFunctions()) {
            //std::cout << "[VM] Found function: " << call_lib_name + funcName << std::endl; // Debug output
            FunctionPtr funcPtr = reinterpret_cast<FunctionPtr>(getFunction(funcName.c_str()));

            if (funcPtr) {
                outerFunctions[call_lib_name + funcName] = funcPtr;
            }
            else {
                std::cerr << "[VM] Function not found in library: " << funcName << std::endl;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[VM] Exception while calling listFunctions: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "[VM] Unknown exception while calling listFunctions." << std::endl;
    }
}

#ifdef __cplusplus
}
#endif