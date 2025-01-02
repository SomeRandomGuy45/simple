#ifndef TOKEN_H
#define TOKEN_H

#include <fstream>
#include <string>
#include <vector>
#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace simple {

// The Token class manages the conversion of script lines to bytecode.
// It holds data about the current file, current line, and related operations.

class Token {
public:
    Token() = default; // Default constructor
    Token(const std::string& fileToRun,
                   const std::string& fileOutput = "!",
                   bool ShouldKill = false); // Constructor with file path
    ~Token(); // Destructor
    void ReOpenFile(const std::string& fileToRun); // Reopen the specified file
    void StartReadingFile(); // Start reading the input file

private:
    VM* vm = new VM(); // Pointer to the VM instance

    std::string FilePath; // File path to the current file
    std::string FileOutput; // Output file path for bytecode (optional)
    std::string currentLine; // Current line being processed
    std::string currentVarHandle; // Current variable handle
    std::string fullString; // Full string of one var
    std::fstream currentFile; // Stream for the current script file
    std::fstream currentBytecodeFile; // Stream for the bytecode output file
    std::vector<std::string> scriptLines; // Lines read from the script file


    bool shouldKill = false;
    bool inString = false;
    bool isInWhileLoop = false;

    // Private helper methods can be declared here
    void processScriptLines(); // Process each line from the script
    void handleLine(std::string& line, int64_t currentLine, bool& trapInFunction); // Handle specific types of lines
    void processFunctionCall(const std::smatch& match); // Handle function calls
    void handleFunctionDefinition(const std::string& line, bool& trapInFunction); // Handle function definitions
    void handleLibraryAddition(const std::string& line, int64_t currentLine); // Handle library additions
    void handleVariableAssignment(const std::string& line, int64_t currentLine); // Handle variable assignments
};

}

#ifdef __cplusplus
}
#endif

#endif // TOKEN_H