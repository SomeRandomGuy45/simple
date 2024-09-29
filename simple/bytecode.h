#pragma once
#ifndef DEFINE_FLAGS_H
#define DEFINE_FLAGS_H
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

extern std::unordered_map<std::string, std::pair<bool, std::string>> defineFlags;

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