#pragma once
#include "expression.h"
#include "types.h"
#include <vector>

enum class SError
{
	ERROR_SS,
	ERROR_RT
};

struct MemoryValue
{
	Expression* e;
	Pointer* p;
	std::string ptr_name;
	MemoryValue(Expression* e, Pointer* p, std::string ptr_name)
		: e(e), p(p), ptr_name(ptr_name)
	{
	}
};

class SmallStep
{
	static std::string global_error;
	static int step_number;

	static std::vector<MemoryValue> memory;

	static bool NameExistMemory(std::string& name, Expression** mv);
	static Expression* Substitution(VariableName* param_name, Expression* body, Expression* argument);
	static LambdaFunction* LambdaFromDeclaration(FunctionDeclaration* fd);
	static Expression* CopyExpression(Expression* exp);
public:
	static Expression* Step(Expression* ast);
	static void PrintError(SError error_type);
};