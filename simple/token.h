#ifndef TOKEN
#define TOKEN
#pragma once
#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

//The token class!
//This holds all the data of some stuff about change it to bytecode
//Such as current file, current line, and etc...

class Token {
public:
	Token() = default;
	Token(std::string fileToRun);
	~Token();
	void ReOpenFile(std::string fileToRun);
	void StartReadingFile();
private:
	VM* vm = new VM();

	std::string currentLine;
	std::fstream currentFile;
	std::fstream currentBytecodeFile;

	std::vector<std::string> scriptLines;
};

#ifdef __cplusplus
}
#endif

#endif