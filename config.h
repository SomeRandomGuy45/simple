#ifndef CONFIG
#define CONFIG
#pragma once

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

#include "helper.h"
#ifdef __cplusplus
extern "C" {
#endif
// Config Inspiration from nlohmann json!
// https://github.com/nlohmann/json/

#ifndef SKIP_SIMPLE_VERSION_CHECK
#if defined(SIMPLE_MAJOR_VERSION) && defined(SIMPLE_MINOR_VERSION) && defined(SIMPLE_BUGFIX_VERSION)
#warning "Warning! You already including a different version of simple!"
#endif
#endif

#ifdef __cplusplus
    #if __cplusplus >= 201103L
    #define nullptr nullptr
    #else
    #define nullptr NULL
    #endif
#else
    #define nullptr NULL
#endif

#define SIMPLE_MAJOR_VERSION 1
#define SIMPLE_MINOR_VERSION 0
#define SIMPLE_BUGFIX_VERSION 0

#define SIMPLE_FULL_VERSION "v" TO_STRING(SIMPLE_MAJOR_VERSION) "." TO_STRING(SIMPLE_MINOR_VERSION)"." TO_STRING(SIMPLE_BUGFIX_VERSION)

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifdef __cplusplus
}
#endif
#endif