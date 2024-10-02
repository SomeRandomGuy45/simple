#ifndef VM_
#define VM_
#include "config.h"
#include "bytecode.h"

class VM {
public:
	//Why try to create a VM without the path to the compiled file
	VM() = default;
	VM(std::string src);
	void changeFilePath(std::string src);
	void Compile();
private:
	std::unordered_map<std::string, std::function<ReturnType(std::vector<std::string>)>> funcNames = returnAllFuncName();
	std::string filePath;
	std::string currentLine;
	std::vector<std::string> scriptLines;
};
#endif //VM