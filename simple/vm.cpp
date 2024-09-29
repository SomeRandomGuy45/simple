#include "vm.h"

VM::VM(std::string src)
{
	filePath = src;
}

void VM::changeFilePath(std::string src)
{
	filePath = src;
}
//This is the implementation of how we can run the code from the bytecode!
void VM::Compile()
{
	std::ifstream file(filePath);
	std::unordered_map<std::string, std::string> var_names;
	bool stuckInComment = false;
	bool skipStatment = false;
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
		if (lineData[0] == "END")
		{
			//we are done with the statement!
			skipStatment = false;
		}
		if (lineData[0] == "/*") {
			stuckInComment = true;
		}
		else if (lineData[0] == "*/") {
			stuckInComment = false;
		}
		if (stuckInComment)
		{
			continue;
		}
		if (lineData[0] == "IFOP") {
			std::string op1 = removeWhitespace(lineData[1], false);
			std::string op2 = removeWhitespace(lineData[3], false);

			// Replace with actual variable values if they exist
			op1 = var_names.count(op1) ? var_names[op1] : op1;
			op2 = var_names.count(op2) ? var_names[op2] : op2;

			// Map operators to lambda functions for comparisons
			std::unordered_map<std::string, std::function<bool(const std::string&, const std::string&)>> comparisonOps = {
				{"==", std::equal_to<std::string>()},
				{"~=", std::not_equal_to<std::string>()},
				{">", std::greater<std::string>()},
				{">=", std::greater_equal<std::string>()},
				{"<", std::less<std::string>()},
				{"<=", std::less_equal<std::string>()}
			};

			// Use the comparison operator to determine skipStatment
			auto it = comparisonOps.find(lineData[2]);
			if (it != comparisonOps.end()) {
				skipStatment = !it->second(op1, op2); // Negate for skipStatment logic
			}
		}
		if (skipStatment)
		{
			continue;
		} 
		else if (lineData[0] == "DEFINEVAR")
		{
			lineData[2].erase(std::remove(lineData[2].begin(), lineData[2].end(), '\"'), lineData[2].end());
			var_names[lineData[1]] = lineData[2];
		}
		else if (lineData[0] == "LOADLIB")
		{
			loadLibrary(lineData[1]);
		}
		else if (lineData[0] == "RUNFUNC")
		{
			std::vector<std::string> args;
			for (const auto& [func_Name, func] : funcNames)
			{
				if (func_Name == lineData[1])
				{
					if (lineData.size() > 2)
					{
						for (size_t i = 1; i < lineData.size(); i++)
						{
							if (lineData[i] == func_Name || lineData[i] == "EOF")
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
						ReturnType result = func(args);
						if (std::holds_alternative<std::string>(result)) {
							//TODO something...
						}
					}
				}
			}
			for (const auto& [func_name, func] : outerFunctions)
			{
				if (func_name == lineData[1])
				{
					std::vector<std::string> args;
					if (lineData.size() > 2)
					{
						for (size_t i = 1; i < lineData.size(); i++)
						{
							if (lineData[i] == func_name || lineData[i] == "EOF")
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
	file.close(); 
}
