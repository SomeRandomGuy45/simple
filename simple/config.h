#pragma once
#include "helper.h"
// Config Inspiration from nlohmann json!
// https://github.com/nlohmann/json/

#ifndef SKIP_SIMPLE_VERSION_CHECK
#if defined(SIMPLE_MAJOR_VERSION) && defined(SIMPLE_MINOR_VERSION) && defined(SIMPLE_BUGFIX_VERSION)
#warning "Warning! You already including a different version of simple!"
#endif
#endif

#define SIMPLE_MAJOR_VERSION 1
#define SIMPLE_MINOR_VERSION 0
#define SIMPLE_BUGFIX_VERSION 0
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define SIMPLE_FULL_VERSION "v" TOSTRING(SIMPLE_MAJOR_VERSION) "." TOSTRING(SIMPLE_MINOR_VERSION) "." TOSTRING(SIMPLE_BUGFIX_VERSION)