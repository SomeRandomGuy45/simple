#ifndef VM_
#define VM_
#include "config.h"
#include "bytecode.h"
#include "table/Node.h"

#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

class VM {
public:
	//Why try to create a VM without the path to the compiled file
	VM() = default;
	VM(std::string src);
	Node parse(std::string input);
	void RunScriptFunction(std::string func_name, std::vector<std::string> args);
	void changeFilePath(std::string src);
	void Compile(std::string customData = "");
	void AddVariable(std::string name, std::string value);
	void RemoveVariable(std::string name);
	std::string RunFuncWithArgs(std::vector<std::string> args, std::string lineData, bool& isFunc);
private:
	std::unordered_map<std::string, std::function<ReturnType(std::vector<std::string>)>> funcNames = returnAllFuncName();
	std::string filePath;
	std::string currentLine;
	std::vector<std::string> scriptLines;

	std::unordered_map<std::string, std::string> var_names;
	std::unordered_map<std::string, std::string> functions;
	std::unordered_map<std::string, std::string> functions_args;

	uint64_t totalRAM = getTotalRAM();
};

#ifdef __cplusplus
}
#endif
#endif //VM