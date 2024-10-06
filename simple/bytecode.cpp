#include "bytecode.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
* Here is where you can set up a define flag
* The format of these flags are like this
* {"name_of_flag", {read_only, "default value"}}
* If a define flag is read only then trying the enable it will not work
*/

std::unordered_map<std::string, std::pair<bool, std::string>> defineFlags = {
    {"MIN_VERSION", {false, "1.0.0"}},
    {"MIN_RAM", {false, "512"}},
	{"OS", {false, "ANY"}},
};

/*
* Different types of bytecodes in a simple std::vector
* This will be used in the VM and the bytecode write function!
*/
std::vector<std::string> bytecodes = {
	"LOADLIB", //Loads required lib
	"MINVERSION", //Min Version
	"MINRAM", //Min Ram
	"RUNFUNC", //Run Function
	"DEFINEVAR", //Defines a variable
	"END", //End of a statement
	"IFOP", //If statement
	"RUNANDDEFVAR", //Does a function and define a variable
};

#ifdef __cplusplus
}
#endif
