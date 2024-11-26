#include "vm.h"

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
		if (token == "EOF")
		{
			continue;
		}
        result.push_back(token);
    }

    return result;
}

void change_line(std::string& str) {
    size_t pos = 0;
    while ((pos = str.find("\\n", pos)) != std::string::npos) {
        str.replace(pos, 2, "\n");
        pos += 1; // Move past the replacement
    }
}

void VM::DoLogic(VM* v)
{
	for (const auto& [key, value] : functions) {
		if (value.empty()) continue;
		v->AddFunction(key.substr(key.find(".") + 1), value);
	}
	for (const auto& [key, value] : functions_args) {
		v->AddFunctionArgs(key.substr(key.find(".") + 1), value);
	}
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
	DoLogic(vm);
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

std::variant<std::string, std::nullptr_t> VM::RunFuncWithArgs(std::vector<std::string> args, std::string lineData, bool& isFunc)
{
	std::string returnVal;
	for (std::string& arg : args)
	{
		arg.erase(std::remove(arg.begin(), arg.end(), '\"'), arg.end());
	}
	for (const auto& [func_Name, func] : funcNames)
	{
		if (func_Name == lineData)
		{
			isFunc = true;
			if (args.size() > 0)
			{
				for (size_t i = 0; i < args.size(); i++)
				{
					if (var_names.count(args[i]) != 0)
					{
						args[i] = var_names[args[i]];
					}
				}
			}
			ReturnType result = func(args);
			if (std::holds_alternative<std::string>(result)) {
				return std::get<std::string>(result);
			}
			else
			{
				return "";
			}
		}
	}
	for (const auto& [func_name, func] : outerFunctions)
	{
		if (args.size() > 0)
		{
			for (size_t i = 0; i < args.size(); i++)
			{
				if (var_names.count(args[i]) != 0)
				{
					args[i] = var_names[args[i]];
				}
			}
		}
		if (func_name == lineData)
		{
			ReturnType result = func(args);
			if (std::holds_alternative<std::string>(result)) {
				return std::get<std::string>(result);
			}
			else
			{
				return "";
			}
		}
	}
	return nullptr;
}
//This is the implementation of how we can run the code from the bytecode!
void VM::Compile(std::string customData, std::string moduleName)
{
	bool stuckInComment = false;
	bool skipStatment = false;
	bool isElseif = false;
	std::string currentFunc = "";
	if (customData.empty()) {
		if (filePath.empty())
		{
			filePath = getRandomFileName();
		}
		std::ifstream file(filePath);
		while (std::getline(file, currentLine)) {
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
				arg = arg.substr(1, arg.size() - 2); // Strip outer quotes
			}
			if (arg.size() >= 2 && arg[arg.size() - 2] == '\\' && arg[arg.size() - 1] == 'n') {
				arg.erase(arg.size() - 2);  // Remove the last two characters
			}
			change_line(arg);
			lineData.push_back(arg); 
		}
		if (lineData[0] == "END" || lineData[1] == "END")
		{
			//we are done with the statement!
			skipStatment = false;
			isElseif = false;
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
		if (lineData[1] == "ENDFUNC")
		{
			if (functions[currentFunc].empty())
			{
				functions.erase(currentFunc);
				functions_args.erase(currentFunc);
			}
			currentFunc = "";
			continue;
		}
		if (lineData[0] == "IFOP" || (lineData[0] == "ELSEIFOP" && skipStatment == true)) {
			std::string op1 = removeWhitespace(lineData[1], false);
			std::string op2 = removeWhitespace(lineData[3], false);
			bool op1_is_func = false;
			bool op2_is_func = false;

			try {
				std::string op3 = op1.substr(0,op1.find("->("));
				std::vector<std::string> args1; // Vector to store arguments
				size_t start = op1.find("->(") + 3; // Skip "->("
				size_t end = op1.find(")", start); // Find closing parenthesis
				if (start != std::string::npos && end != std::string::npos) {
					std::string arg = op1.substr(start, end - start);
					std::stringstream ss(arg);
					while (std::getline(ss, arg, ',')) {
						// Trim whitespace around the argument
						arg.erase(0, arg.find_first_not_of(" \t\n"));
						arg.erase(arg.find_last_not_of(" \t\n") + 1);
						if (!arg.empty()) {
							args1.push_back(arg); // Add non-empty argument to the vector
						}
					}
				}
				std::string op4 = op2.substr(0,op2.find("->("));
				std::vector<std::string> args2; // Vector to store arguments
				start = op2.find("->(") + 3; // Skip "->("
				end = op2.find(")", start); // Find closing parenthesis
				if (start != std::string::npos && end != std::string::npos) {
					std::string arg = op2.substr(start, end - start);
					std::stringstream ss_2(arg);
					while (std::getline(ss_2, arg, ',')) {
						// Trim whitespace around the argument
						arg.erase(0, arg.find_first_not_of(" \t\n"));
						arg.erase(arg.find_last_not_of(" \t\n") + 1);
						if (!arg.empty()) {
							args2.push_back(arg); // Add non-empty argument to the vector
						}
					}
				}
				std::variant<std::string, std::nullptr_t> value_1 = RunFuncWithArgs(args1, op3, op1_is_func);
				std::variant<std::string, std::nullptr_t> value_2 = RunFuncWithArgs(args2, op4, op2_is_func);
				if (std::holds_alternative<std::string>(value_1))
				{
					op1 = std::get<std::string>(value_1);
				}
				if (std::holds_alternative<std::string>(value_2))
				{
					op2 = std::get<std::string>(value_2);
				}
			} catch (...) {
				// add something here later idc
			}	

			// Replace with actual variable values if they exist and its not a function
			op1 = (var_names.count(op1) != 0 && op1_is_func == false) ? var_names[op1] : op1;
			op2 = (var_names.count(op2) != 0 && op2_is_func == false) ? var_names[op2] : op2;

			// Map operators to lambda functions for comparisons
			std::unordered_map<std::string, std::function<bool(const std::string&, const std::string&)>> comparisonOps = {
				{"==", std::equal_to<std::string>()},
				{"~=", std::not_equal_to<std::string>()},
				{"!>", std::greater<std::string>()},
				{">=", std::greater_equal<std::string>()},
				{"<!", std::less<std::string>()},
				{"<=", std::less_equal<std::string>()}
			};

			// Use the comparison operator to determine skipStatment
			auto it = comparisonOps.find(lineData[2]);
			if (it != comparisonOps.end()) {
				skipStatment = !it->second(op1, op2); // Negate for skipStatment logic
				if (lineData[0] == "ELSEIFOP" && skipStatment) {
					isElseif = !skipStatment;
				}
			}
			//ifStatementValues.push_back(skipStatment);
		} else if (lineData[0] == "ELSE") {
			if (skipStatment && !isElseif)
			{
				skipStatment = false;
				continue;
			}

			skipStatment = true;
            continue;
        }
		if (skipStatment)
		{
			continue;
		}
		else if (lineData[0] == "BEGINFUN")
		{
			if (!moduleName.empty())
			{
				lineData[1] = moduleName + "." + lineData[1];
			}
			functions[lineData[1]] = "";;
			for (size_t i = 2; i < lineData.size(); ++i)
			{
				functions_args[lineData[1]] += lineData[i];
				if (lineData[i] != "EOF")
				{
					functions_args[lineData[1]] += ",";
				}
			}
			currentFunc = lineData[1];
		}
		else if (lineData[0] == "DOFUNCCALL")
		{
			std::string stuffAdd;
			for (size_t i = 0; i < lineData.size(); i++)
			{
				if (functions[lineData[i]] == "DOFUNCCALL")
				{
					continue;
				}
				stuffAdd += lineData[i] + ((lineData[i] == "EOF" || lineData[i] == "END") ? " " : ",");
			}
			functions[currentFunc] += stuffAdd + "\n";
		}
		else if (lineData[0] == "LOADSLIB") {
			std::string libPath = std::filesystem::current_path().string() + "/" + lineData[1] + ".sbcc";
			if (!std::filesystem::exists(libPath))
			{
				std::cerr << "[ERROR]: Library file not found: " << libPath << std::endl;
                return;
			}
			VM* vm = new VM(libPath);
			vm->Compile("", lineData[1]);
		    for (const auto& [name, value] : vm->functions)
			{
				functions[name] = value;
			}
			for (const auto& [name, value] : vm->functions_args)
			{
				functions_args[name] = value;
			}
			for (const auto& [name, value] : vm->var_names)
			{
				var_names[name] = value;
			}
			delete vm;
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
				if (SIMPLE_FULL_VERSION < lineData[2])
				{
					std::cerr << "Error: Requested minimum version (" << lineData[2] << ") is not equal to the current version (" << SIMPLE_FULL_VERSION << ")" << std::endl;
                    return;
				}
			}
			else if (lineData[1] == "MAX_VERSION")
			{
				if (SIMPLE_FULL_VERSION > lineData[2])
                {
                    std::cerr << "Error: Requested maximum version (" << lineData[2] << ") is not equal to the current version (" << SIMPLE_FULL_VERSION << ")" << std::endl;
                    return;
                }
			}
			//add more later on
		} 
		else if (lineData[0] == "DEFINEVAR")
		{
			if (!moduleName.empty())
			{
				lineData[1] = moduleName + "." + lineData[1];
			}
			lineData[2].erase(std::remove(lineData[2].begin(), lineData[2].end(), '\"'), lineData[2].end());
			std::vector<std::string> args;
			std::string temp = lineData[2].substr(0, lineData[2].find("->"));
			std::string data = lineData[2];
			size_t pos = data.find("->(");
			if (pos != std::string::npos) {
				data = data.substr(pos + 3); // Extract substring after "->("
				data.erase(std::remove(data.begin(), data.end(), ')'), data.end()); // Remove closing ')'
			}
			std::stringstream ss2(data);
			while (std::getline(ss2, arg, '+')) {
				// Trim whitespace around the argument
				arg.erase(arg.find_last_not_of(" \t\n") + 1);
				arg.erase(0, arg.find_first_not_of(" \t\n")); // Trim leading whitespace
				if (arg.size() > 1 && 
					((arg.front() == '"' && arg.back() == '"') || (arg.front() == '\'' && arg.back() == '\''))) {
					arg = arg.substr(1, arg.size() - 2); // Strip outer quotes
				}
				if (arg.size() >= 2 && arg[arg.size() - 2] == '\\' && arg[arg.size() - 1] == 'n') {
					arg.erase(arg.size() - 2);  // Remove the last two characters
				}
				change_line(arg);
				args.push_back(arg); 
			}
			if (lineData.size() > 0)
			{
				for (size_t i = 2; i < lineData.size(); i++)
				{
					if (((lineData[i] == lineData[1] && var_names.count(lineData[i]) == 0)) == true)
					{
						continue;
					}
					if (lineData[i] == "EOF")
					{
						continue;
					}
					std::string backUpVar = lineData[i];
					if (lineData[i].front() == '"' && lineData[i].back() == '"') {
						lineData[i] = lineData[i].substr(1, lineData[i].size() - 2);
					}
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
			if (functions.count(temp) != 0)
			{
				RunScriptFunction(temp, args);
				var_names[lineData[1]] = "";
			}
			else
			{
				var_names[lineData[1]] = lineData[2];
			}
		}
		else if (lineData[0] == "RUNANDDEFVAR")
		{
			if (!moduleName.empty())
			{
				lineData[1] = moduleName + "." + lineData[1];
			}
			std::vector<std::string> args;
			if (lineData.size() > 0)
			{
				for (size_t i = 2; i < lineData.size(); i++)
				{
					if (((lineData[i] == lineData[1] && var_names.count(lineData[i]) == 0)) == true)
					{
						continue;
					}
					if (lineData[i] == "EOF")
					{
						continue;
					}
					std::string backUpVar = lineData[i];
					if (lineData[i].front() == '"' && lineData[i].back() == '"') {
						lineData[i] = lineData[i].substr(1, lineData[i].size() - 2);
					}
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
			if (functions.count(lineData[1]) != 0)
			{
				RunScriptFunction(lineData[2], args);
				var_names[lineData[1]] = "";
			}
			for (const auto& [func_Name, func] : funcNames)
			{
				if (func_Name == lineData[2])
				{
					ReturnType result = func(args);
					std::string returnVal = "";
					if (std::holds_alternative<std::string>(result)) {
						returnVal = std::get<std::string>(result);
					}
					returnVal = removeWhitespace(returnVal, false);
					var_names[lineData[1]] = returnVal;
				}
			}
			for (const auto& [func_name, func] : outerFunctions)
			{
				if (func_name == lineData[2])
				{
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
			if (lineData.size() > 0)
			{
				for (size_t i = 2; i < lineData.size(); i++)
				{
					if (((lineData[i] == lineData[1] && var_names.count(lineData[i]) == 0)) == true)
					{
						continue;
					}
					if (lineData[i] == "EOF")
					{
						continue;
					}
					std::string backUpVar = lineData[i];
					if (lineData[i].front() == '"' && lineData[i].back() == '"') {
						lineData[i] = lineData[i].substr(1, lineData[i].size() - 2);
					}
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
			if (functions.count(lineData[1]) != 0)
			{
				RunScriptFunction(lineData[1], args);
			}
			for (const auto& [func_Name, func] : funcNames)
			{
				if (func_Name == lineData[1])
				{
					func(args);
				}
			}
			for (const auto& [func_name, func] : outerFunctions)
			{
				if (func_name == lineData[1])
				{
					func(args);
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

void VM::AddFunction(std::string func_name, std::string func_value)
{
	functions[func_name] = func_value;
}

void VM::AddFunctionArgs(std::string func_name, std::string func_args)
{
	functions_args[func_name] = func_args;
}

void VM::RemoveFunction(std::string func_name)
{
	functions.erase(func_name);
	functions_args.erase(func_name);
}

#ifdef __cplusplus
}
#endif