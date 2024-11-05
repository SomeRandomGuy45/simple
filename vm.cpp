#include "vm.h"

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }

    return result;
}

Node VM::parse(std::string input)
{
    Node root;
    std::regex entryPattern(R"(\s*\"([^\"]+)\"\s*=\s*(\"([^\"]+)\"|([a-zA-Z_][a-zA-Z0-9_]*|{[^}]*}))\s*)");
    std::smatch matches;

    std::string sanitizedInput = removeComments(input);  // Remove comments first

    std::string::const_iterator searchStart(sanitizedInput.cbegin());
    while (std::regex_search(searchStart, sanitizedInput.cend(), matches, entryPattern)) {
        std::string key = matches[1];
        std::string value = matches[3].matched ? matches[3] : matches[4]; // Capture value from either string or variable

        // If the value is a variable, check if it's defined in the table
        if (matches[4].matched) {
            if (var_names.find(value) != var_names.end()) {
                value = root[value]; // Resolve variable to its value
            }
        }

        root[key] = value;
        searchStart = matches.suffix().first;
    }

    return root;
}

#ifdef __cplusplus
extern "C" {
#endif

VM::VM(std::string src)
{
	filePath = src;
}

void VM::RunScriptFunction(std::string func_name, std::vector<std::string> args)
{
	if (functions.count(func_name) == 0 && functions_args.count(func_name) == 0) {return;}
	std::vector<std::string> funcargs = split(functions_args[func_name], ',');
	if (funcargs.size() != args.size())
	{
		std::cout << "[WARN] Arg size is over " << funcargs.size() << ". Some args will be dropped\n";
	}
	for (auto& [key, data] : functions) {
    	if (key != func_name) {
            continue;
        }
        size_t pos;
        while ((pos = data.find("DOFUNCCALL,")) != std::string::npos) {
			data.erase(pos, std::string("DOFUNCCALL,").length());
        }
    }
	VM* vm = new VM();
	for (size_t i = 0; i < funcargs.size(); ++i) {
		if (i < args.size())
		{
			if (args[i] == func_name)
			{
				continue;
			}
			vm->AddVariable(funcargs[i], args[i]);
		}
	}
	vm->Compile(functions[func_name]);
	delete vm;
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
void VM::Compile(std::string customData)
{
	bool stuckInComment = false;
	bool skipStatment = false;
	std::string currentFunc = "";
	if (customData == "")
	{
		std::ifstream file(filePath);
		if (!file.is_open())
		{
			throw std::runtime_error("Error couldn't open compiled file with path: " + filePath);
		}
		while (std::getline(file, currentLine))
		{
			scriptLines.push_back(currentLine);
		}
		file.close();
	}
	else
	{
		std::stringstream ss(customData);
		while (std::getline(ss, currentLine))
		{
			scriptLines.push_back(currentLine);
		}
	}
	for (const std::string& line : scriptLines) {
		std::stringstream ss(line);
		std::string arg;
		std::vector<std::string> lineData;
		while (std::getline(ss, arg, ',')) {
			// Trim whitespace around the argument
			arg.erase(arg.find_last_not_of(" \t\n") + 1);
			arg.erase(0, arg.find_first_not_of(" \t\n")); // Trim leading whitespace

			if (arg.size() > 1 && 
				((arg.front() == '"' && arg.back() == '"') || (arg.front() == '\'' && arg.back() == '\''))) {
				// Remove the outermost quotes if they are the same
				arg = arg.substr(1, arg.size() - 2); // Remove outer quotes
			}
			lineData.push_back(arg); 
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

			try {
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
				if (args1.size() > 1) {
					op1 = RunFuncWithArgs(args1, op3);
				}
				if (args2.size() > 1) {
					op2 = RunFuncWithArgs(args2, op4);
				}
			} catch (...) {
				// Handle some stuff idk
			}

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
		else if (lineData[0] == "BEGINFUN")
		{
			functions[lineData[1]] = "";
			functions_args[lineData[1]] = lineData[2];
			currentFunc = lineData[1];
		}
		else if (lineData[0] == "DOFUNCCALL")
		{
			if (lineData[1] == "END")
			{
				currentFunc = "";
				continue;
			}
			std::string stuffAdd;
			for (size_t i = 0; i < lineData.size(); i++)
			{
				if (functions[lineData[i]] == "DOFUNCCALL")
				{
					continue;
				}
				stuffAdd += lineData[i] + ((lineData[i] == "EOF" || lineData[i] == "END") ? " " : ",");
			}
			functions[currentFunc] = stuffAdd + "\n";
		}
		else if (lineData[0] == "DEFTOP")
		{
			lineData[1] = removeWhitespace(lineData[1], false);
			if (defineFlags.count(lineData[1]) == 0)
			{
				continue;
			}
			if (defineFlags[lineData[1]].first == true)
			{
				continue;
			}
			defineFlags[lineData[1]].second = lineData[2];
			if (lineData[1] == "MIN_RAM")
			{
				uint64_t minRam;
				try {
					minRam = std::stoull(lineData[2]);
				} catch (...) {
					minRam = 512;
				}
				if (minRam > totalRAM)
				{
					std::cerr << "Error: Requested minimum RAM (" << minRam << ") is greater than available RAM (" << totalRAM << ")" << std::endl;
					return;
				}
			}
			else if (lineData[1] == "MIN_VERSION")
			{
				if (SIMPLE_FULL_VERSION != lineData[2])
				{
					std::cerr << "Error: Requested minimum version (" << lineData[2] << ") is not equal to the current version (" << SIMPLE_FULL_VERSION << ")" << std::endl;
                    return;
				}
			}
			//add more later on
		} 
		else if (lineData[0] == "DEFINEVAR")
		{
			lineData[2].erase(std::remove(lineData[2].begin(), lineData[2].end(), '\"'), lineData[2].end());
			var_names[lineData[1]] = lineData[2];
		}
		else if (lineData[0] == "RUNANDDEFVAR")
		{
			std::vector<std::string> args;
			if (functions.count(lineData[1]) != 0)
			{
				std::vector<std::string> args;
				if (lineData.size() > 0)
				{
					for (size_t i = 2; i < lineData.size(); i++)
					{
						if (((lineData[i] == lineData[1] && var_names.count(lineData[i]) == 0)) == true)
						{
							continue;
						}
						std::string backUpVar = lineData[i];
						lineData[i].erase(std::remove(lineData[i].begin(), lineData[i].end(), '\"'), lineData[i].end());
						if (!lineData[i].empty() && var_names.count(lineData[i]) == 1) {
							for (const auto& var : var_names)
							{
								if (var.first == lineData[i]) {
                                    args.push_back(var.second);  // Use the stored value if found
                                }
							}
						} else {
							args.push_back(backUpVar);  // Use the original string if not found
						}
					}
				}
				RunScriptFunction(lineData[2], args);
			}
			for (const auto& [func_Name, func] : funcNames)
			{
				if (func_Name == lineData[2])
				{
					if (lineData.size() > 0)
					{
						for (size_t i = 2; i < lineData.size(); i++)
						{
							if ((lineData[i] == func_Name || (lineData[i] == lineData[1] && var_names.count(lineData[i]) == 0)) == true)
							{
								continue;
							}
							std::string backUpVar = lineData[i];
							lineData[i].erase(std::remove(lineData[i].begin(), lineData[i].end(), '\"'), lineData[i].end());
							if (!lineData[i].empty() && var_names.count(lineData[i]) == 1) {
								for (const auto& var : var_names)
								{
									if (var.first == lineData[i]) {
                                        args.push_back(var.second);  // Use the stored value if found
                                    }
								}
							} else {
								args.push_back(backUpVar);  // Use the original string if not found
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
						for (size_t i = 2; i < lineData.size(); i++)
						{
							if ((lineData[i] == func_name || (lineData[i] == lineData[1] && var_names.count(lineData[i]) == 0)) == true)
							{
								continue;
							}
							std::string backUpVar = lineData[i];
							lineData[i].erase(std::remove(lineData[i].begin(), lineData[i].end(), '\"'), lineData[i].end());
							if (!lineData[i].empty() && var_names.count(lineData[i]) == 1) {
								for (const auto& var : var_names)
								{
									if (var.first == lineData[i]) {
                                        args.push_back(var.second);  // Use the stored value if found
                                    }
								}
							} else {
								args.push_back(backUpVar);  // Use the original string if not found
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
			if (functions.count(lineData[1]) != 0)
			{
				std::vector<std::string> args;
				if (lineData.size() > 0)
				{
					for (size_t i = 2; i < lineData.size(); i++)
					{
						if (lineData[i] == "EOF")
						{
							continue;
						}
						std::string backUpVar = lineData[i];
						lineData[i].erase(std::remove(lineData[i].begin(), lineData[i].end(), '\"'), lineData[i].end());
						if (!lineData[i].empty() && var_names.count(lineData[i]) == 1) {
							for (const auto& var : var_names)
							{
								if (var.first == lineData[i]) {
                                    args.push_back(var.second);  // Use the stored value if found
                                }
							}
						} else {
							args.push_back(backUpVar);  // Use the original string if not found
						}
					}
				}
				RunScriptFunction(lineData[1], args);
			}
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
							std::string backUpVar = lineData[i];
							lineData[i].erase(std::remove(lineData[i].begin(), lineData[i].end(), '\"'), lineData[i].end());

							if (!lineData[i].empty() && var_names.count(lineData[i]) == 1) {
								for (const auto& var : var_names)
								{
									if (var.first == lineData[i]) {
                                        args.push_back(var.second);  // Use the stored value if found
                                    }
								}
							} else {
								args.push_back(backUpVar);  // Use the original string if not found
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
							std::string backUpVar = lineData[i];
							lineData[i].erase(std::remove(lineData[i].begin(), lineData[i].end(), '\"'), lineData[i].end());

							if (!lineData[i].empty() && var_names.count(lineData[i]) == 1) {
								for (const auto& var : var_names)
								{
									if (var.first == lineData[i]) {
                                        args.push_back(var.second);  // Use the stored value if found
                                    }
								}
							} else {
								args.push_back(backUpVar);  // Use the original string if not found
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
}

void VM::AddVariable(std::string name, std::string value)
{
	var_names[name] = value;
}

void VM::RemoveVariable(std::string name)
{
	var_names.erase(name);
}

#ifdef __cplusplus
}
#endif