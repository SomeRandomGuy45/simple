#include "sys.h"

bool is_using_default_stdout = true;
bool is_using_default_stderr = true;

void __set_output(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "[SYS] Error: Invalid number of arguments\n";
        return;
    }
    FILE* file = nullptr;
    if (args[0] == "stdout") {
        file = stdout;
        is_using_default_stdout = false;
    } else if (args[0] == "stderr") {
        file = stderr;
        is_using_default_stderr = false;
    } else {
        std::cerr << "[SYS] Error: Invalid stream specified\n";
        return;
    }

    const std::string& filename = args[1];
    if (filename != "default") {
        if (freopen(filename.c_str(), "a+", file) == nullptr) {
            std::cerr << "[SYS] Error: Failed to redirect " << args[0] << " to file " << filename << "\n";
        }
    } else {
        if (freopen(stdCall, "w", file) == nullptr) {
            std::cerr << "[SYS] Error: Failed to restore " << args[0] << " to default\n";
        } else {
            if (file == stdout) {
                is_using_default_stdout = true;
            } else if (file == stderr) {
                is_using_default_stderr = true;
            }
        }
    }
}

open DLLEXPORT ReturnType helper set_output(const std::vector<std::string>& args) {
    __set_output(args);
    return nullptr;
}

open DLLEXPORT std::vector<std::string> helper listFunctions() {
    return {"set_output"};
}

open DLLEXPORT FunctionPtr helper getFunction(const char* name) {
    if (std::string(name) == "set_output") {
        return &set_output;
    }
    return nullptr;
}

open DLLEXPORT std::vector<std::string> helper listVars()
{
    return {"is_using_default_stdout", "is_using_default_stderr"};
}

open DLLEXPORT VarType helper getVariable(const char *name)
{
    std::string varName(name);
    if (varName == "is_using_default_stdout") {
        std::string returnValue = is_using_default_stdout ? "true" : "false";
        return returnValue;
    } else if (varName == "is_using_default_stderr") {
        std::string returnValue = is_using_default_stderr ? "true" : "false";
        return returnValue;
    }
    return "";
}
