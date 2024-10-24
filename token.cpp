#include "token.h"
#include <regex>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#ifndef _WIN32
#include <unistd.h>
#endif

std::string getCurrentWorkingDirectory() {
    #ifdef _WIN32
    // Wide string buffer for Windows
    wchar_t wbuffer[FILENAME_MAX];
    if (GetCurrentDirectoryW(FILENAME_MAX, wbuffer) == 0) {
        throw std::runtime_error("Error: Unable to get current working directory");
    }

    // Convert the wide string (wchar_t*) to a standard string (std::string)
    char buffer[FILENAME_MAX];
    wcstombs(buffer, wbuffer, FILENAME_MAX); // Wide char to multibyte char conversion
    return std::string(buffer);
    
    #else
    // POSIX (Linux/macOS) way to get the current working directory
    char buffer[FILENAME_MAX];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        throw std::runtime_error("Error: Unable to get current working directory");
    }
    return std::string(buffer);
    #endif
}

#ifdef __cplusplus
extern "C" {
#endif

// Constructor
Token::Token(const std::string &fileToRun, const std::string &fileOutput, bool ShouldKill)
{
    shouldKill = ShouldKill;
    FileOutput = fileOutput;
    if (!std::filesystem::exists(fileToRun)) {
        std::ofstream file(fileToRun);
        file.close();
    }
    currentFile.open(fileToRun);
    if (!currentFile.is_open()) {
        std::cerr << "[TOKEN-OPEN] Error: Failed to open file with path: " + fileToRun << "\n";
        throw std::runtime_error("Failed to run script!");
    }
}

// Reopens the file or opens a new one with path
void Token::ReOpenFile(const std::string& fileToRun) {
    currentFile.close();
    currentFile.open(fileToRun);
    if (!currentFile.is_open()) {
        std::cerr << "[TOKEN-OPEN] Error: Failed to open file with path: " + fileToRun << "\n";
        throw std::runtime_error("Failed to run script!");
    }
}

// Destructor
Token::~Token() {
    if (currentFile.is_open()) {
        currentFile.close();
    }
    if (currentBytecodeFile.is_open()) {
        currentBytecodeFile.close();
    }
    for (auto& pointer_Val : allocatedBlocks)
    {
        std::free(pointer_Val);
    }
}

// Start reading the input file
void Token::StartReadingFile() {
    std::srand(static_cast<unsigned int>(std::time(0)));
    if (FileOutput == "!")
    {
        FilePath = std::filesystem::temp_directory_path().string() + getRandomFileName();
    }
    else
    {
        std::string ending = "";
        if (std::filesystem::exists(FileOutput) && std::filesystem::is_directory(FileOutput))
        {
            ending = "/";
        }
        else
        {
            ending = ".sbcc";
        }
        FileOutput = FileOutput == "" ? getCurrentWorkingDirectory() + "/" + getRandomFileName() : getCurrentWorkingDirectory() + "/" + FileOutput + ending;
        FilePath = ending == "/" ? FileOutput + getRandomFileName() : FileOutput;   
    }
    if (!std::filesystem::exists(FilePath)) {
        std::ofstream file(FilePath);
        file.close();
    }
    currentBytecodeFile.open(FilePath);
    if (!currentBytecodeFile.is_open()) {
        std::cerr << "[TOKEN-OPEN] Error: Failed to open .sbcc file with path: " + FilePath << "\n";
        throw std::runtime_error("Failed to run script!");
    }

    while (std::getline(currentFile, currentLine)) {
        scriptLines.push_back(currentLine);
    }

    processScriptLines();
}

// Process each script line
void Token::processScriptLines() {
    int64_t currentLine = 0;
    bool trapInComment = false;
    bool trapInFunction = false;

    for (std::string& line : scriptLines) {
        currentLine += 1;
        if (line.empty()) {
            continue;
        }

        if (line.substr(0, 2) == "/*") {
            trapInComment = true;
            continue;
        }
        if (line.substr(0, 2) == "*/") {
            trapInComment = false;
        }

        if (line.substr(0, 2) == "//") {
            continue;
        }

        if (trapInComment) {
            continue;
        }

        if (trapInFunction) {
            currentBytecodeFile << "DOFUNCCALL,";
        }

        line = removeWhitespace(removeComments(line), false);
        handleLine(line, currentLine, trapInFunction);
    }

    if (shouldKill)
    {
        return;
    }

    vm->changeFilePath(FilePath);
    vm->Compile();
}

// Handle different types of lines
void Token::handleLine(std::string& line, int64_t currentLine, bool& trapInFunction) {
    std::smatch match;
    // Function call
    if (std::regex_match(line, match, std::regex(R"(\s*(\w+(?:\.\w+)?)\s*\((.*?)\))"))) {
        processFunctionCall(match);
    }
    // If statement
    else if (std::regex_match(line, match, std::regex(R"(if\s+(.+?)\s*(==|~=|>=|>|<=|<)\s*(.+?)\s*then)"))) {
        currentBytecodeFile << "IFOP," << std::string(match[1]) << "," << std::string(match[2]) << "," << std::string(match[3]) << std::endl;
    }
    // Else If statement
    else if (std::regex_match(line, match, std::regex(R"(elseif\s+(.+?)\s*(==|~=|>=|>|<=|<)\s*(.+?)\s*then)"))) {
        currentBytecodeFile << "ELSEIFOP," << std::string(match[1]) << "," << std::string(match[2]) << "," << std::string(match[3]) << std::endl;
    }
    else if (line.substr(0,4) == "else") {
        currentBytecodeFile << "ELSE" << std::endl;
    }
    // Local variable assignment
    else if (std::regex_match(line, match, std::regex(R"(local\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^()]*)\))"))) {
        currentBytecodeFile << "RUNANDDEFVAR," + std::string(match[1]) + "," + std::string(match[2]) + "," + std::string(match[3]) << std::endl;
    }
    else if (std::regex_match(line, match, std::regex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*=\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^()]*)\))"))) {
        currentBytecodeFile << "RUNANDDEFVAR," + std::string(match[1]) + "," + std::string(match[2]) + "," + std::string(match[3]) << std::endl;
    }
    // Other cases...
    else if (line.substr(0, 7) == "!define") {
        if (std::regex_match(line, match, std::regex(R"(!define\s+(.*)\s+(.*))"))) {
            currentBytecodeFile << "DEFTOP," + std::string(match[1]) + "," + std::string(match[2]) << std::endl;
        }
    }
    else if (line.substr(0, 5) == "local") {
        if (std::regex_match(line, match, std::regex("local\\s+(\\w+)\\s*=\\s*(?:\"([^\"]*(?:\\n[^\"]*)*)\"|([^\"\\n]+(?:\\n[^\"\\n]*)*))"))) {
            currentBytecodeFile << "DEFINEVAR," + std::string(match[1]) + "," + (match[2].str().empty() ? match[3].str() : match[2].str()) << std::endl;
        }
    }
    else if (line.substr(0, 5) == "func!") {
        handleFunctionDefinition(line, trapInFunction);
    }
    else if (line.substr(0, 3) == "add") {
        handleLibraryAddition(line, currentLine);
    }
    else if (line.substr(0, 3) == "end") {
        if (line.substr(0, 4) == "end!") {
            trapInFunction = false;
        }
        currentBytecodeFile << "END" << std::endl;
    }
    else if (line.substr(0, 2) == "//" || line.substr(0, 2) == "*/") {
        // Skip comments
        return;
    }
    else {
        handleVariableAssignment(line, currentLine);
    }
}

void Token::processFunctionCall(const std::smatch& match) {
    std::string funcName = match[1];
    std::string argsStr = match[2];
    std::string line = "RUNFUNC," + funcName + "," + argsStr + ",EOF";
    currentBytecodeFile << line << std::endl;
}

void Token::handleFunctionDefinition(const std::string& line, bool& trapInFunction) {
    std::smatch match;
    if (std::regex_match(line, match, std::regex(R"(func!\s+([a-zA-Z_]\w*)\s*\(([^)]*)\))"))) {
        std::string funcName = match[1];
        std::string argsStr = match[2];
        std::string add = "BEGINFUN," + funcName + "," + argsStr + ",EOF";
        currentBytecodeFile << add << std::endl;
        trapInFunction = true;
    }
}

void Token::handleLibraryAddition(const std::string& line, int64_t currentLine) {
    std::string libName = LIBPATH + line.substr(4) + LIB_EXT;
    if (!std::filesystem::exists(libName)) {
        std::cerr << "[TOKEN-PARSER] Couldn't find library: " + line.substr(4) + ". Line: " + line + ". Line index is " + std::to_string(currentLine) << ". Trying access lib: " << libName << "\n";
        throw std::runtime_error("Failed to run script!");
    }
    currentBytecodeFile << "LOADLIB," + libName << std::endl;
}

void Token::handleVariableAssignment(const std::string& line, int64_t currentLine) {
    std::smatch match;
    if (std::regex_match(line, match, std::regex(R"((\w+)\s*=\s*(.+))"))) {
        currentBytecodeFile << "DEFINEVAR," + std::string(match[1]) + "," + std::string(match[2]) << std::endl;
    } else {
        std::cerr << "[TOKEN-PARSER] Couldn't find bytecode with this line: " + line + ". Line index is " + std::to_string(currentLine) << "\n";
        throw std::runtime_error("Failed to run script!");
    }
}

#ifdef __cplusplus
}
#endif