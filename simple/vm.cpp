#include "vm.h"

VM::VM(std::string src)
{
	filePath = src;
}


void VM::changeFilePath(std::string src)
{
	filePath = src;
}

void VM::Compile()
{
	std::ifstream file(filePath);
	std::unordered_map<std::string, std::string> var_names;
	bool stuckInComment = false;
	if (!file.is_open())
	{
		throw std::runtime_error("Error couldn't open compiled file with path: " + filePath);
	}
	while (std::getline(file, currentLine))
	{
		scriptLines.push_back(currentLine);
	}
	for (const std::string& line : scriptLines) {
		std::stringstream ss(line);
		std::string arg;
		std::vector<std::string> lineData;
		while (std::getline(ss, arg, ',')) {
			// Trim whitespace around the argument
			arg.erase(0, arg.find_first_not_of(" \t\n"));
			arg.erase(arg.find_last_not_of(" \t\n") + 1);
			if (!arg.empty()) {
				lineData.push_back(arg); // Add non-empty argument to the vector
			}
		}
		if (lineData[0] == "/*") {
			stuckInComment = true;
		}
		else if (lineData[0] == "*/") {
			stuckInComment = false;
		}
		else if (lineData[0] == "DEFINEVAR")
		{
			if (stuckInComment)
			{
				continue;
			}
			lineData[2].erase(std::remove(lineData[2].begin(), lineData[2].end(), '\"'), lineData[2].end());
			var_names[lineData[1]] = lineData[2];
		}
		else if (lineData[0] == "LOADLIB")
		{
			if (stuckInComment)
			{
				continue;
			}
			loadLibrary(lineData[1]);
		}
		else if (lineData[0] == "RUNFUNC")
		{
			if (stuckInComment)
			{
				continue;
			}
			std::string funcName = "";
			for (const auto& [func_Name, func] : funcNames)
			{
				if (func_Name == lineData[1])
				{
					funcName = func_Name;
					std::vector<std::string> args;
					if (lineData.size() > 2)
					{
						for (size_t i = 1; i < lineData.size(); i++)
						{
							if (lineData[i] == funcName || lineData[i] == "EOF")
							{
								continue;
							}
							lineData[i].erase(std::remove(lineData[i].begin(), lineData[i].end(), '\"'), lineData[i].end());
							if (var_names.find(lineData[i]) != var_names.end())
							{
								args.push_back(var_names.find(lineData[i])->second);
							}
							else
							{
								args.push_back(lineData[i]);
							}
						}
					}
					ReturnType result = func(args);
					if (std::holds_alternative<std::string>(result)) {
						//TODO something...
					}
				}
			}
			for (const auto& [func_name, func] : outerFunctions)
			{
				if (stuckInComment)
				{
					continue;
				}
				if (func_name == lineData[1])
				{
					funcName = func_name;
					std::vector<std::string> args;
					if (lineData.size() > 2)
					{
						for (size_t i = 1; i < lineData.size(); i++)
						{
							if (lineData[i] == funcName || lineData[i] == "EOF")
							{
								continue;
							}
							lineData[i].erase(std::remove(lineData[i].begin(), lineData[i].end(), '\"'), lineData[i].end());
							if (var_names.find(lineData[i]) != var_names.end())
							{
								args.push_back(var_names.find(lineData[i])->second);
							}
							else
							{
								args.push_back(lineData[i]);
							}
						}
					}
					ReturnType result = func(args);
					if (std::holds_alternative<std::string>(result)) {
						//TODO something...
					}
				}
			}
		}
	}
}
