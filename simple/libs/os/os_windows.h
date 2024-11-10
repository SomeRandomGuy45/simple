/*
2D drawing lib for windows
*/
#ifndef OS_HEADER_MAIN
#define OS_HEADER_MAIN
#ifdef _WIN32
#include <windows.h>
#include "os_header.h"

// TODO

ReturnType create_window(std::vector<std::string> args);

#endif
#endif