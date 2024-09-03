// main.cpp
#include "interpreter.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // Check if the script file argument is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <script_file>" << std::endl;
        return 1;
    }

    // Get the script file name from command-line arguments
    std::string scriptFile = argv[1];

    // Initialize the Interpreter
    Interpreter interpreter;

    // Load and execute the script file
    interpreter.executeFromFile(scriptFile);

    return 0;
}