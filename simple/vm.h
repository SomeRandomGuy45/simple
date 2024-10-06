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
	void changeFilePath(std::string src);
	void Compile();
	std::string RunFuncWithArgs(std::vector<std::string> args, std::string lineData);
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