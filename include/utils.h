#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "expression.h"

class Exception
{
	std::string message;
public:
	Exception(const char* msg)
		: message(msg)
	{
	}
	std::string& GetMessage() { return this->message; }
};

void assert(bool exp, const char* msg);
void PrintOperator(Operator op);
void PrintProgram(Expression* e);