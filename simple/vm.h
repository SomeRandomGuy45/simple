#ifndef VM_
#define VM_
#include "config.h"
#include "bytecode.h"

#include <variant>
#include <algorithm>
#include <tuple>

#ifdef __cplusplus
extern "C" {
#endif

namespace simple {

extern std::unordered_map<std::string, std::string> functions_module;
extern std::unordered_map<std::string, std::string> functions_module_args;
extern std::unordered_map<std::string, std::string> var_module_names;

class VM {
public:
	//Why try to create a VM without the path to the compiled file
	VM() = default;
	VM(std::string src);

	void DoLogic(VM* v);
	void changeFilePath(std::string src);
	void Compile(std::string customData = "", std::string moduleName = "", bool isWhileLoop_ = false);
	void AddVariable(std::string name, std::string value);
	void RemoveVariable(std::string name);
	void AddFunction(std::string func_name, std::string func_value);
	void AddFunctionArgs(std::string func_name, std::string func_args);
	void RemoveFunction(std::string func_name);
	void AddWhileLoop(int num, std::string value);
	void AddWhileLoop_Arg(int num, std::tuple<std::string, std::string, std::string> value);
	std::variant<double, std::nullptr_t> evaluateExpression(const std::string& expr);
	std::string RunScriptFunction(std::string func_name, std::vector<std::string> args);
	std::vector<std::string> DoStringLogic(const std::string& line1, const std::string& line2);
	
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
	std::unordered_map<int, std::string> whileLoops;
	std::unordered_map<int, std::tuple<std::string, std::string, std::string>> whileLoops_args;

	uint64_t totalRAM = getTotalRAM();
};

}

#ifdef __cplusplus
}
#endif
#endif //VM