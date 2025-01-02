#include "vm.h"
#include "tinyexpr.h"

#include <any>

namespace simple {

std::unordered_map<std::string, std::string> functions_module;
std::unordered_map<std::string, std::string> functions_module_args;
std::unordered_map<std::string, std::string> var_module_names;

bool breakCurrentLoop = false;

std::vector<std::string> VM::DoStringLogic(const std::string& line1, const std::string& line2)
{
	std::string op1 = removeWhitespace(line1, false);
	std::string op2 = removeWhitespace(line2, false);
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
	return {op1, op2};
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
	for (const auto& [key, value] : functions_module) {
		v->AddFunction(key, value);
	}
	for (const auto& [key, value] : functions_module_args) {
		v->AddFunctionArgs(key, value);
	}
	for (const auto& [key, value] : var_module_names) {
		v->AddVariable(key, value);
	}
	for (const auto& [key, value] : var_names) {
		v->AddVariable(key, value);
	}
}

#ifdef __cplusplus
extern "C" {
#endif

std::string formatNumber(double value) {
    std::ostringstream oss;
    oss << value; // Convert number to string
    std::string result = oss.str();

    // Remove trailing zeros and the decimal point if unnecessary
    if (result.find('.') != std::string::npos) {
        // Erase trailing zeros
        result.erase(result.find_last_not_of('0') + 1);

        // If the last character is a decimal point, remove it
        if (result.back() == '.') {
            result.pop_back();
        }
    }

    return result;
}
std::variant<double, std::nullptr_t> VM::evaluateExpression(const std::string& expr) {
    std::unordered_map<std::string, double> variables;
    
    for (const auto& [var, val_str] : var_names) {
        if (expr == var) {
            return nullptr;
        }
    }

    try {
        // Convert variables from strings to doubles
        for (const auto& [var, val_str] : var_names) {
            try {
                double val = std::stod(val_str); // Convert string to double
                variables[var] = val;
            } catch (const std::invalid_argument&) {
                continue;
            } catch (const std::out_of_range&) {
                continue;
            }
        }

        // Prepare variables for TinyExpr
        std::vector<te_variable> te_vars;
        for (const auto& [var, val] : variables) {
            te_vars.push_back({var.c_str(), &variables[var]});
        }

        // Parse and evaluate the expression
        int error_index = 0;
        te_expr* compiled_expr = te_compile(expr.c_str(), te_vars.data(), te_vars.size(), &error_index);

        if (!compiled_expr) {
            return nullptr; // Invalid expression
        }

        double result = te_eval(compiled_expr);
        te_free(compiled_expr); // Free the compiled expression

        return result;
    } catch (...) {
        return nullptr;
    }
}

VM::VM(std::string src)
{
	filePath = src;
}

std::string VM::RunScriptFunction(std::string func_name, std::vector<std::string> args)
{
	if (functions.count(func_name) == 0 && functions_args.count(func_name) == 0) {return "";}
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
	std::string output = vm->returnValue;
	delete vm;
	return output;
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
	if (funcNames.count(lineData) != 0)
	{
		isFunc = true;
		ReturnType result = funcNames[lineData](args);
		if (std::holds_alternative<std::string>(result)) {
			return std::get<std::string>(result);
		}
		else
		{
			return "";
		}
	}
	else if (outerFunctions.count(lineData) != 0)
	{
		isFunc = true;
		ReturnType result = outerFunctions[lineData](args);
		if (std::holds_alternative<std::string>(result)) {
			return std::get<std::string>(result);
		}
		else
		{
			return "";
		}
	}
	else if (functions.count(lineData) != 0) {
		isFunc = true;
		std::string returnValue = RunScriptFunction(lineData, args);
		return returnValue;
	}
	return nullptr;
}
//This is the implementation of how we can run the code from the bytecode!
void VM::Compile(std::string customData, std::string moduleName)
{
	bool stuckInComment = false;
	bool skipStatment = false;
	bool isElseif = false;
	bool isWhileLoop = false;
	int currentForLoop = 0;
	std::string currentFunc = "";
	std::vector<std::any> allocatedObjects;
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
			std::variant<double, std::nullptr_t> val = evaluateExpression(arg);
			if (std::holds_alternative<double>(val) and !isNumeric(arg)) {
				//std::cout << std::get<double>(val) << " : " << arg << "\n";
				lineData.push_back(formatNumber(std::get<double>(val)));
				continue;
			}
			change_line(arg);
			lineData.push_back(arg); 
			allocatedObjects.push_back(arg);
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
			if (!currentFunc.empty()) {
				if (functions[currentFunc].empty())
				{
					functions.erase(currentFunc);
					functions_args.erase(currentFunc);
				}
				if (!moduleName.empty())
				{
					functions_module[currentFunc] = functions[currentFunc];
					functions_module_args[currentFunc] = functions_args[currentFunc];
				}
				currentFunc = "";
			}
			if (isWhileLoop) {
				isWhileLoop = false;
				VM* vm = new VM();
				DoLogic(vm);
				while (!breakCurrentLoop) {
					vm->Compile(whileLoops[currentForLoop]);
					if (breakCurrentLoop) {
                        break;
					}
				}
				for (const auto& [name, value] : vm->var_names)
				{
					var_names[name] = value;
				}
				delete vm;
				breakCurrentLoop = false;
				currentForLoop += 1;
			}
			continue;
		}
		if (lineData[0] == "IFOP" || (lineData[0] == "ELSEIFOP" && skipStatment == true)) {
			std::vector<std::string> args = DoStringLogic(lineData[1], lineData[3]);
			std::string op1 = args[0];
			std::string op2 = args[1];
			// Map operators to lambda functions for comparisons
			std::unordered_map<std::string, std::function<bool(const std::string&, const std::string&)>> comparisonOps = {
				{"==", [](const std::string& a, const std::string& b) {	
					if (isNumeric(a) && isNumeric(b)) {
						return std::stod(a) == std::stod(b);
					} else {
						return a == b;
					}
				 }},
				{"~=", [](const std::string& a, const std::string& b) { 
					if (isNumeric(a) && isNumeric(b)) {
						return std::stod(a) != std::stod(b);
					} else {
						return a != b;
					}
				 }},
				{"<", [](const std::string& a, const std::string& b) { return std::stoi(a) < std::stoi(b); }},
				{">", [](const std::string& a, const std::string& b) { return std::stoi(a) > std::stoi(b); }},
				{"<=", [](const std::string& a, const std::string& b) { return std::stoi(a) <= std::stoi(b); }},
				{">=", [](const std::string& a, const std::string& b) { return std::stoi(a) >= std::stoi(b); }}
			};
			// Use the comparison operator to determine skipStatment
			auto it = comparisonOps.find(lineData[2]);
			 if (it != comparisonOps.end()) {
				skipStatment = !it->second(op1, op2); // Negate for skipStatment logic
				if (lineData[0] == "ELSEIFOP" && skipStatment == true) {
					isElseif = !skipStatment;
				}
			}
			//ifStatementValues.push_back(skipStatment);
		} 
		else if (lineData[0] == "ELSE") 
		{
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
		else if (lineData[0] == "WHILELOOPCALL") {
			if (isWhileLoop == false) {
				continue;
			}
			std::string stuffAdd;
			for (size_t i = 0; i < lineData.size(); i++)
			{
				if (lineData[i] == "WHILELOOPCALL")
				{
					continue;
				}
				stuffAdd += lineData[i] + ((lineData[i] == "EOF" || lineData[i] == "END") ? " " : ",");
			}
			whileLoops[currentForLoop] += stuffAdd + "\n";
			continue;
		}
		else if (lineData[0] == "WHILE") {
			std::vector<std::string> args = DoStringLogic(lineData[1], lineData[3]);
			std::string op1 = args[0];
			std::string op2 = args[1];
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
				whileLoops[currentForLoop] = "";
				whileLoops_args[currentForLoop] = {op1, lineData[2], op2};
				isWhileLoop = it->second(op1, op2); // Negate for skipStatment logic
			}
		}
		else if (lineData[0] == "BREAK") {
			breakCurrentLoop = true;
			break;
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
		else if (lineData[0] == "RETURN") 
		{
			std::vector<std::string> args;
			std::string temp = lineData[1].substr(0, lineData[1].find("->"));
			std::string data = lineData[1];
			bool FoundFunc = false;
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
			if (functions.count(temp) != 0)
			{
				returnValue = RunScriptFunction(temp, args);
				FoundFunc = true;
			}
			if (funcNames.count(temp) != 0)
			{
				ReturnType result = funcNames[temp](args);
				if (std::holds_alternative<std::string>(result))
				{
					returnValue = std::get<std::string>(result);
				}
				else
				{
					returnValue = "";
				}
				returnValue = removeWhitespace(returnValue, false);
				FoundFunc = true;
			}
			if (outerFunctions.count(temp) != 0)
			{
				ReturnType result = outerFunctions[temp](args);
				if (std::holds_alternative<std::string>(result))
				{
					returnValue = std::get<std::string>(result);
				}
				else
				{
					returnValue = "";
				}
				returnValue = removeWhitespace(returnValue, false);
				FoundFunc = true;
			}
			if (var_names.count(lineData[1]) != 0)
			{
				returnValue = var_names[lineData[1]];
				FoundFunc = true;
			}
			if (FoundFunc)
			{
				continue;
			}
			returnValue = lineData[1];
			break;
		}
		else if (lineData[0] == "LOADSLIB") {
			std::string libPath = std::filesystem::current_path().string() + "/" + lineData[1] + ".sbcc";
			if (!std::filesystem::exists(libPath))
			{
				std::cerr << "[ERROR]: Library file not found: " << libPath << std::endl;
                break;
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
			std::string var_data = var_names.count(lineData[2]) != 0 ? var_names[lineData[2]] : lineData[2];
			var_names[lineData[1]] = var_data;
			if (!moduleName.empty())
			{
				var_module_names[lineData[1]] = var_names[lineData[1]];
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
				for (size_t i = 3; i < lineData.size(); i++)
				{
					if (((lineData[i] == lineData[1] && var_names.count(lineData[i]) == 0)) == true)
					{
						continue;
					}
					if (lineData[i] == "EOF")
					{
						continue;
					}
					bool didUseFunction = false;
					std::string backUpVar = lineData[i];
					try {
						std::string functionName = lineData[i].substr(0,lineData[i].find("->("));
						std::vector<std::string> args; // Vector to store arguments
						size_t start = lineData[i].find("->(") + 3; // Skip "->("
						size_t end = lineData[i].find(")", start); // Find closing parenthesis
						if (start != std::string::npos && end != std::string::npos) {
							std::string arg = lineData[i].substr(start, end - start);
							std::stringstream ss(arg);
							while (std::getline(ss, arg, ',')) {
								// Trim whitespace around the argument
								arg.erase(0, arg.find_first_not_of(" \t\n"));
								arg.erase(arg.find_last_not_of(" \t\n") + 1);
								if (!arg.empty()) {
									args.push_back(arg); // Add non-empty argument to the vector
								}
							}
						}
						std::variant<std::string, std::nullptr_t> value = RunFuncWithArgs(args, functionName, didUseFunction);
						if (std::holds_alternative<std::string>(value))
						{
							backUpVar = std::get<std::string>(value);
						}
					} catch (...) {};
					if ((lineData[i].front() == '"' && lineData[i].back() == '"') || (lineData[i].front() == '\'' && lineData[i].back() == '\'')) {
						lineData[i] = lineData[i].substr(1, lineData[i].size() - 2);
					}
					if (!lineData[i].empty() && var_names.count(lineData[i]) == 1 && didUseFunction == false) {
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
			if (functions.count(lineData[2]) != 0)
			{
				var_names[lineData[1]] = RunScriptFunction(lineData[2], args);
			}
			if (funcNames.count(lineData[2]) != 0)
			{
				ReturnType result = funcNames[lineData[2]](args);
				std::string returnVal = "";
				if (std::holds_alternative<std::string>(result)) {
					returnVal = std::get<std::string>(result);
				}
				returnVal = removeWhitespace(returnVal, false);
				var_names[lineData[1]] = returnVal;
			}
			if (outerFunctions.count(lineData[2]))
			{
				ReturnType result = outerFunctions[lineData[2]](args);
				std::string returnVal = "";
				if (std::holds_alternative<std::string>(result)) {
					returnVal = std::get<std::string>(result);
				}
				returnVal = removeWhitespace(returnVal, false);
				var_names[lineData[1]] = returnVal;
			}
			if (!moduleName.empty())
			{
				var_module_names[lineData[1]] = var_names[lineData[1]];
			}
		}
		else if (lineData[0] == "LOADLIB")
		{
			std::string libName = "";
			bool foundLib = false;
			for (const auto& file : std::filesystem::recursive_directory_iterator(std::filesystem::current_path())) {
				if (file.path().filename().string() == "lib" + lineData[1] + LIB_EXT && file.is_regular_file()) {
                    //std::cout << file.path().string() << "\n";
					libName = file.path().string();
					foundLib = true;
                    break;
                }
			}
			if (std::filesystem::exists(expandHomeDirectory(LIBPATH + lineData[1] + LIB_EXT)) && libName.empty()) {
				libName = expandHomeDirectory(LIBPATH + lineData[1] + LIB_EXT);
				foundLib = true;
			}
			if (!foundLib) {
				std::cerr << "[VM] Couldn't find library in LIBPATH and current directory: " + lineData[1] + ". Trying access lib: " << libName << "\n";
			}
			loadLibrary(libName);
			for (auto& [name, value] : Return_OuterVariables())
			{
				var_names[removeWhitespace(name, false)] = value;
				//std::cout << var_names[removeWhitespace(name, false)] << "\n";
			}
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
					bool didUseFunction = false;
					std::string backUpVar = lineData[i];
					try {
						std::string functionName = lineData[i].substr(0,lineData[i].find("->("));
						std::vector<std::string> args; // Vector to store arguments
						size_t start = lineData[i].find("->(") + 3; // Skip "->("
						size_t end = lineData[i].find(")", start); // Find closing parenthesis
						if (start != std::string::npos && end != std::string::npos) {
							std::string arg = lineData[i].substr(start, end - start);
							std::stringstream ss(arg);
							while (std::getline(ss, arg, ',')) {
								// Trim whitespace around the argument
								arg.erase(0, arg.find_first_not_of(" \t\n"));
								arg.erase(arg.find_last_not_of(" \t\n") + 1);
								if (!arg.empty()) {
									args.push_back(arg); // Add non-empty argument to the vector
								}
							}
						}
						std::variant<std::string, std::nullptr_t> value = RunFuncWithArgs(args, functionName, didUseFunction);
						if (std::holds_alternative<std::string>(value))
						{
							backUpVar = std::get<std::string>(value);
						}
					} catch (...) {};
					if ((lineData[i].front() == '"' && lineData[i].back() == '"') || (lineData[i].front() == '\'' && lineData[i].back() == '\'')) {
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
			if (funcNames.count(lineData[1]) != 0)
			{
				funcNames[lineData[1]](args);
			}
			if (outerFunctions.count(lineData[1]) != 0)
			{
				outerFunctions[lineData[1]](args);
			}
		} else if (lineData[0] == "INC") {
			std::string arg_1 = lineData[1];
			std::string arg_2 = lineData[2];
			bool foundVar = false;
			std::unordered_map<std::string, std::function<std::string(const std::string&, const std::string&)>> ops = {
				{"++", [](const std::string& a, const std::string& b) { return std::to_string(std::stod(a) + 1); }},
				{"--", [](const std::string& a, const std::string& b) { return std::to_string(std::stod(a) - 1); }},
			};
			for (const auto& [var, key] : var_names) {
				if (var == arg_1) {
					foundVar = true;
					break;
				}
			}
			if (!foundVar) {
				continue;
			}
			var_names[arg_1] = ops[arg_2](var_names[arg_1], arg_2);
		}
	}
	for (std::any& obj : allocatedObjects) {
		if (obj.type() == typeid(std::string*)) {
			delete std::any_cast<std::string*>(obj);
		} else if (obj.type() == typeid(VM*)) {
			delete std::any_cast<VM*>(obj);
		}
	}
	allocatedObjects.clear();
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

}

#ifdef __cplusplus
}
#endif