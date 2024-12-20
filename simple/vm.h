#ifndef VM_
#define VM_
#include "config.h"
#include "bytecode.h"
#include "table/Node.h"

#include <variant>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

extern std::unordered_map<std::string, std::string> functions_module;
extern std::unordered_map<std::string, std::string> functions_module_args;
extern std::unordered_map<std::string, std::string> var_module_names;

class VM {
public:
	//Why try to create a VM without the path to the compiled file
	VM() = default;
	VM(std::string src);
	Node parse(std::string input);

	void DoLogic(VM* v);
	std::string RunScriptFunction(std::string func_name, std::vector<std::string> args);
	void changeFilePath(std::string src);
	void Compile(std::string customData = "", std::string moduleName = "");
	void AddVariable(std::string name, std::string value);
	void RemoveVariable(std::string name);
	void AddFunction(std::string func_name, std::string func_value);
	void AddFunctionArgs(std::string func_name, std::string func_args);
	void RemoveFunction(std::string func_name);
	
	std::string returnValue;
	
	std::variant<std::string, std::nullptr_t> RunFuncWithArgs(std::vector<std::string> args, std::string lineData, bool& isFunc);
	
	std::unordered_map<std::string, std::string> functions;
	std::unordered_map<std::string, std::string> functions_args;
	std::unordered_map<std::string, std::string> var_names = {
		// some default vars
		{"true", "true"},
		{"false", "false"},
        {"NULL", "NULL"},
		{"OS", OS}
	};
private:

	std::unordered_map<std::string, std::function<ReturnType(std::vector<std::string>)>> funcNames = returnAllFuncName();
	std::string filePath;
	std::string currentLine;
	std::vector<std::string> scriptLines;

	uint64_t totalRAM = getTotalRAM();
};

#ifdef __cplusplus
}
#endif
#endif //VM