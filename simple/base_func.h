#pragma once
#include "helper.h"
#include "bytecode.h"

void print(std::vector<std::string> args)
{
	for (const auto& val : args)
	{
		std::cout << val << " ";
	}
	std::cout << "\n";
}