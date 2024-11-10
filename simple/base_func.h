#pragma once
#ifndef BASE_FUNC
#define BASE_FUNC

#include "helper.h"
#include <functional>
#include <variant>
#include <unordered_map>
#include <string>
#include <vector>
#include "libs/libs.h"

#ifdef __cplusplus
extern "C" {
#endif



typedef std::string(*FunctionPtr)(const std::vector<std::string>& args);

extern std::unordered_map<std::string, FunctionPtr> outerFunctions;

extern std::unordered_map<std::string, void*> loadedLibraries;

extern std::vector<void*> allocatedBlocks;

std::unordered_map<std::string, std::function<ReturnType(std::vector<std::string>)>> returnAllFuncName();

void loadLibrary(const std::string& libName);

#ifdef __cplusplus
}
#endif

#endif // BASE_FUNC