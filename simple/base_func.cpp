#include "base_func.h"
#include "tiny_obj_loader.h"

/*
* With functions you can return 2 types
* 1. nullptr
* 2. std::string
*/


std::unordered_map<std::string, FunctionPtr> outerFunctions{};
std::unordered_map<std::string, void*> loadedLibraries{};

ReturnType print(std::vector<std::string> args)
{
	for (const auto& val : args)
	{
		std::cout << val << " ";
	}
	std::cout << "\n";
    return nullptr;
}

ReturnType write(std::vector<std::string> args)
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

//The holder of all the functions
//This looks trash and I really need to find a different way lol
std::unordered_map<std::string, std::function<ReturnType(std::vector<std::string>)>> returnAllFuncName() {
	return std::unordered_map<std::string, std::function<ReturnType(std::vector<std::string>)>> {
		{"print", [](std::vector<std::string> args) -> ReturnType { return print(args); }},
        {"writeToFile", [](std::vector<std::string> args) -> ReturnType { return write(args); }},
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