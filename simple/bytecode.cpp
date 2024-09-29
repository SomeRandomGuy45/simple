#include "bytecode.h"

std::unordered_map<std::string, std::pair<bool, std::string>> defineFlags = {
    {"MIN_VERSION", {false, "1.0.0"}},
    {"MIN_RAM", {false, "512"}}
};

std::vector<std::string> bytecodes = {
	"LOADLIB", //Loads required lib
	"MINVERSION", //Min Version
	"MINRAM", //Min Ram
	"RUNFUNC", //Run Function
	"DEFINEVAR", //Defines a variable
	"COMMENT", //Just a comment
};
