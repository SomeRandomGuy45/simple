#include "vm.h"

VM::VM(std::string src)
{
	filePath = src;
}

void VM::changeFilePath(std::string src)
{
	filePath = src;
}

std::string VM::RunFuncWithArgs(std::vector<std::string> args, std::string lineData)
{
	std::string returnVal;
	for (std::string& arg : args)
	{
		arg = arg.substr(0, arg.size() - 1);
		arg.erase(std::remove(arg.begin(), arg.end(), '\"'), arg.end());
	}
	for (const auto& [func_Name, func] : funcNames)
	{
		if (func_Name == lineData)
		{
			if (args.size() > 0)
			{
				for (size_t i = 1; i < args.size(); i++)
				{
					if (var_names.find(args[i]) != var_names.end())
					{
						args[i] = var_names[args[i]];
					}
				}
			}
			ReturnType result = func(args);
			if (std::holds_alternative<std::string>(result)) {
				returnVal = std::get<std::string>(result);
			}
		}
	}
	for (const auto& [func_name, func] : outerFunctions)
	{
		if (args.size() > 0)
		{
			for (size_t i = 1; i < args.size(); i++)
			{
				if (var_names.find(args[i]) != var_names.end())
				{
					args[i] = var_names[args[i]];
				}
			}
		}
		if (func_name == lineData)
		{
			ReturnType result = func(args);
			if (std::holds_alternative<std::string>(result)) {
				returnVal = std::get<std::string>(result);
			}
		}
	}
	return returnVal;
}
//This is the implementation of how we can run the code from the bytecode!
void VM::Compile()
{
	std::ifstream file(filePath);
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

			std::string op3 = op1.substr(0,op1.find("("));
			std::vector<std::string> args1; // Vector to store arguments
			std::stringstream ss(op1.substr(op1.find("(") + 1, op1.find(")") - 2));
			std::string arg_1;
			while (std::getline(ss, arg_1, ',')) {
				// Trim whitespace around the argument
				arg_1.erase(0, arg_1.find_first_not_of(" \t\n"));
				arg_1.erase(arg_1.find_last_not_of(" \t\n") + 1);
				if (!arg.empty()) {
					args1.push_back(arg_1); // Add non-empty argument to the vector
				}
			}
			std::string op4 = op2.substr(0,op2.find("("));
			std::vector<std::string> args2; // Vector to store arguments
			std::stringstream ss_2(op1.substr(op2.find("(") + 1, op2.find(")") - 2));
			std::string arg;
			while (std::getline(ss_2, arg, ',')) {
				// Trim whitespace around the argument
				arg.erase(0, arg.find_first_not_of(" \t\n"));
				arg.erase(arg.find_last_not_of(" \t\n") + 1);
				if (!arg.empty()) {
					args2.push_back(arg); // Add non-empty argument to the vector
				}
			}
			op1 = RunFuncWithArgs(args1, op3);
			op2 = RunFuncWithArgs(args2, op4);

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
		else if (lineData[0] == "RUNANDDEFVAR")
		{
			std::vector<std::string> args;
			for (const auto& [func_Name, func] : funcNames)
			{
				if (func_Name == lineData[2])
				{
					if (lineData.size() > 0)
					{
						for (size_t i = 1; i < lineData.size(); i++)
						{
							if (lineData[i] == func_Name || lineData[i] == lineData[1])
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
						std::string returnVal = "";
						if (std::holds_alternative<std::string>(result)) {
							returnVal = std::get<std::string>(result);
						}
						returnVal = removeWhitespace(returnVal, false);
						var_names[lineData[1]] = returnVal;
					}
				}
			}
			for (const auto& [func_name, func] : outerFunctions)
			{
				if (func_name == lineData[2])
				{
					std::vector<std::string> args;
					if (lineData.size() > 0)
					{
						for (size_t i = 1; i < lineData.size(); i++)
						{
							if (lineData[i] == func_name || lineData[i] == lineData[1])
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
					std::string returnVal = "";
					if (std::holds_alternative<std::string>(result)) {
						returnVal = std::get<std::string>(result);
					}
					returnVal = removeWhitespace(returnVal, false);
					var_names[lineData[1]] = returnVal;
				}
			}
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
					if (lineData.size() > 0)
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
							//TODO allow return vals
						}
					}
				}
			}
			for (const auto& [func_name, func] : outerFunctions)
			{
				if (func_name == lineData[1])
				{
					std::vector<std::string> args;
					if (lineData.size() > 0)
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
						//TODO allow return vals
					}
				}
			}
		}
	}
	file.close(); 
}
