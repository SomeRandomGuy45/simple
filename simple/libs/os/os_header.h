#include <variant>
#include <vector>
#include <iostream>
#include <string>

#include "os_header.h"

using ReturnType = std::variant<std::nullptr_t, std::string>;