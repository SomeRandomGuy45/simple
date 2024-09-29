#pragma once
#ifndef BASE_FUNC
#define BASE_FUNC

#include "helper.h"
#include "bytecode.h"
#include <functional>
#include <variant>
#include <unordered_map>
#include <string>
#include <vector>

using ReturnType = std::variant<std::nullptr_t, std::string>;

typedef std::string(*FunctionPtr)(const std::vector<std::string>& args);

extern std::unordered_map<std::string, FunctionPtr> outerFunctions;

extern std::unordered_map<std::string, void*> loadedLibraries;

ReturnType print(std::vector<std::string> args);

std::unordered_map<std::string, std::function<ReturnType(std::vector<std::string>)>> returnAllFuncName();

void loadLibrary(const std::string& libName);

#endif // BASE_FUNC