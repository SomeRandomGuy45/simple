#ifndef HELPER
#define HELPER
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <memory>
#include "base_func.h"

//some helper varibles
#ifdef _WIN32
#include <Windows.h>
#define LIB_EXT ".dll"
#define LOAD_LIB(name) LoadLibraryA(name)
#define UNLOAD_LIB(lib) FreeLibrary((HMODULE)lib)
#define GET_FUNC(lib, func) reinterpret_cast<void*>(GetProcAddress((HMODULE)lib, func))
#define LIBPATH "C:\\Program Files\\simple_libs\\lib"
#define OS "WIN"
#else
#include <dlfcn.h>
#define LIBPATH "/usr/local/lib/simple_libs/lib"
#ifdef __APPLE__
    #define LIB_EXT ".dylib"
    #define OS "APPLE"
#else
    #define LIB_EXT ".so"
    #define OS "OTHER"
#endif
#define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
#define UNLOAD_LIB(lib) dlclose(lib)
#define GET_FUNC(lib, func) dlsym(lib, func)
#endif

// Locations where libs can be placed
// Will add when I figure a install location for simp
// {Location}
extern std::vector<std::string> libLocations;

#ifdef _WIN32
inline uint64_t getTotalRAM() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPhys / 1024.0; // Return RAM in bytes
}
#elif defined(__linux__) // Linux
inline uint64_t getTotalRAM() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            std::string ram_str = line.substr(10); // Extract the memory value part
            long ram_kb = std::stol(ram_str);      // Convert the memory value to long
            return ram_kb * 1024;                  // Convert KB to bytes
        }
    }
    return 0; // In case of an error
}
#elif defined(__APPLE__) && defined(__MACH__) // macOS
#include <sys/sysctl.h>

inline uint64_t getTotalRAM() {
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    uint64_t totalRAM;
    size_t len = sizeof(totalRAM);
    sysctl(mib, 2, &totalRAM, &len, NULL, 0);
    return totalRAM / 1024.0; // Return RAM in bytes
}

#else
#error "Unsupported platform"
#endif

#ifdef __cplusplus
extern "C" {
#endif

std::string removeWhitespace(std::string str, bool NO_SPACE);
std::string getRandomFileName();

#ifdef __cplusplus
}
#endif

#endif
