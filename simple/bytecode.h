#pragma once
#ifndef DEFINE_FLAGS_H
#define DEFINE_FLAGS_H
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/*
* Here is where you can set up a define flag
* The format of these flags are like this
* {"name_of_flag", read_only, "default value"}
* If a define flag is read only then trying the enable it will not work
*/
extern std::unordered_map<std::string, std::pair<bool, std::string>> defineFlags;

/*
* Different types of bytecodes in a simple std::vector
* This will be used in the VM and the bytecode write function!
*/
extern std::vector<std::string> bytecodes;
/*
* The simple bytecode struct!
* It will have these types of data
* bytecode
* actually line
* and value
* Will be used in a later date
*/

struct ByteCode {
	std::string bytecode = "";
	std::string originalLine = "";
	std::string value = "";
};
#endif // DEFINE_FLAGS_H