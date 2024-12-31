#include "random.h"

unsigned int seed = time(NULL);

void _set_seed(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        seed = time(NULL);
        return;
    }
    seed = std::stoul(args[0]);
    return;
}

long _random_number(const std::vector<std::string>& args) {
    srand(seed);
    if (args.size() < 1) {
        std::cerr << "[RANDOM] Error: Invalid number of arguments\n";
        return 0;
    }
    long min = 0;
    try { 
        if (args.size() >= 2) {
            min = std::stol(args[1]);
        }
        long max = std::stol(args[0]);
        if (min >= max) {
            std::cerr << "[RANDOM] Error: Invalid range (min >= max)\n";
            return 0;
        }
        long randomNum = min + rand() % (max+1 - min);
        return randomNum;
    } catch (std::invalid_argument& e) {
        std::cerr << "[RANDOM] Error: Invalid arguments (must be integers)\n";
        return 0;
    }
}

std::string _random_string(const std::vector<std::string>& args) {
    srand(seed);
    if (args.size() < 1) {
        std::cerr << "[RANDOM_STRING] Error: Invalid number of arguments\n";
        return "";
    }
    size_t length = std::stoul(args[0]);
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

open DLLEXPORT ReturnType helper random_number(const std::vector<std::string>& args) {
    return std::to_string(_random_number(args));
}

open DLLEXPORT ReturnType helper random_string(const std::vector<std::string>& args) {
    return _random_string(args);
}

open DLLEXPORT ReturnType helper set_seed(const std::vector<std::string>& args) {
    _set_seed(args);
    return nullptr;
}

open DLLEXPORT std::vector<std::string> helper listFunctions() {
    return {"random_number", "random_string", "set_seed"};
}

open DLLEXPORT FunctionPtr helper getFunction(const char* name) {
    if (std::string(name) == "random_number") {
        return &random_number;
    } else if (std::string(name) == "random_string") {
        return &random_string;
    } else if (std::string(name) == "set_seed") {
        return &set_seed;
    }
    return nullptr;
}