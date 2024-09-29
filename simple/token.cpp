#include "token.h"

//This gets called when we want to run from a file
Token::Token(std::string fileToRun)
{
	if (!std::filesystem::exists(fileToRun))
	{
		std::ofstream file(fileToRun);
		file.close();
	}
	currentFile.open(fileToRun);
	if (!currentFile.is_open())
	{
		std::cerr << "[TOKEN-OPEN] Error: Failed to open file with path: " + fileToRun << "\n";
		throw std::runtime_error("Failed to run script!");
	}
}
//Reopens the file or opens a new one with path
void Token::ReOpenFile(std::string fileToRun)
{
	currentFile.close();
	currentFile.open(fileToRun);
	if (!currentFile.is_open())
	{
		std::cerr << "[TOKEN-OPEN] Error: Failed to open file with path: " + fileToRun << "\n";
		throw std::runtime_error("Failed to run script!");
	}
}

//Get's called once we need to destroy the Token class
//Its best to close the currentFile and currentBytecodeFile or it could lead to somewhere things...
Token::~Token()
{
	if (currentFile.is_open()) {
		currentFile.close();
	}
	if (currentBytecodeFile.is_open()) {
		currentBytecodeFile.close();
	}
}

//This function will read each line of a function and change it into bytecode
void Token::StartReadingFile()
{
	std::srand(static_cast<unsigned int>(std::time(0)));
	std::string FilePath = std::filesystem::temp_directory_path().string() + getRandomFileName();
	std::cout << "[TOKEN-OPEN] Path is: " << FilePath << "\n";
	if (!std::filesystem::exists(FilePath))
	{
		std::ofstream file(FilePath);
		file.close();
	}
	currentBytecodeFile.open(FilePath);
	if (!currentFile.is_open())
	{
		std::cerr << "[TOKEN-OPEN] Error: Failed to open .sbcc file with path: " + FilePath << "\n";
		throw std::runtime_error("Failed to run script!");
	}
	while (std::getline(currentFile, currentLine))
	{
		scriptLines.push_back(currentLine);
	}
	int currentLine = 0;
	for (const std::string& line : scriptLines) {
		std::smatch match;
		std::regex regex(R"(\s*(\w+(?:\.\w+)?)\s*\((.*?)\))");
		if (line.find("local") != std::string::npos) {
			regex = R"(local\s+(\w+)\s*=\s*(.+))";
			if (std::regex_match(line, match, regex)) {
				std::string name = match[1];
				std::string value = match[2];
				currentBytecodeFile << "DEFINEVAR," + name + "," + value << std::endl;
			}
		}
		else if (line.find("add") != std::string::npos) {
			std::string libName = LIBPATH + line.substr(4) + LIB_EXT;
			if (!std::filesystem::exists(libName))
			{
				std::cerr << "[TOKEN-PARSER] Couldn't find library. Line: " + line + ". Line index is " + std::to_string(currentLine) << "\n";
				throw std::runtime_error("Failed to run script!");
			}
			currentBytecodeFile << "LOADLIB," + libName << std::endl;
		}
		else if (std::regex_match(line, match, regex))
		{
			std::string FuncCall = match[0];
			std::string FuncName = match[1];
			std::string argsStr = match[2]; // Capture the arguments string
			std::vector<std::string> args; // Vector to store arguments
			std::stringstream ss(argsStr);
			std::string arg;
			while (std::getline(ss, arg, ',')) {
				// Trim whitespace around the argument
				arg.erase(0, arg.find_first_not_of(" \t\n"));
				arg.erase(arg.find_last_not_of(" \t\n") + 1);
				if (!arg.empty()) {
					args.push_back(arg); // Add non-empty argument to the vector
				}
			}
			std::string line = "RUNFUNC," + FuncName + ",";
			for (const auto& a : args) {
				line += a + ",";
			}
			line += "EOF";
			currentBytecodeFile << line << std::endl;
		}
		else if (line.find("//") != std::string::npos) {
			continue;
		}
		else
		{
			std::cerr << "[TOKEN-PARSER] Couldn't find bytecode with this line: " + line + ". Line index is " + std::to_string(currentLine) << "\n";
			throw std::runtime_error("Failed to run script!");
		}
		currentLine += 1;
	}
}