#pragma once
#include <iostream>
#include <string>
#include "types.h"

enum Operator
{
	O_PLUS	,
	O_MINUS	,
	O_MULT	,
	O_DIV	,
	O_EQUALS,
	O_LTHAN	,
	O_GTHAN	,
	O_LETHAN,
	O_GETHAN
};

enum class ExpressionID
{
	E_VAR_NAME,					// BEG VALUES
	E_LAMBDA_FUNCTION,			// *
	E_FUNCTION_APPLICATION,		// *
	E_OPERATION,				// *
	E_INTEGER,					// *
	E_BOOLEAN,					// *
	E_POINTER,					// *
	E_SKIP,						// *
	E_ERROR,					// END VALUES

	E_UNARY_INC,
	E_UNARY_DEC,
	E_FUNCTION_DECLARATION,
	E_IF_ELSE,
	E_VAR_DECLARATION,
	E_DEREFERENCE,
	E_TRY_CATCH,
	E_SEQUENCE
};

struct Expression
{
	bool isValue;		// n | b | _(T1:x) T2 { e } | p | error
	ExpressionID expID;
	Type type;
	Expression(bool isValue, ExpressionID eid)
		: isValue(isValue), expID(eid)
	{
	}

	virtual ~Expression(){}
};

void DeleteExpression(Expression* ex);

struct VariableName : Expression
{
	std::string name;
	VariableName(std::string name)
		: Expression(false, ExpressionID::E_VAR_NAME), name(name)
	{
	}
	virtual ~VariableName(){}
};

struct LambdaFunction : Expression
{
	VariableName* argIdentifier;
	Expression* body;

	LambdaFunction(VariableName* argIdent, Expression* body)
		: Expression(true, ExpressionID::E_LAMBDA_FUNCTION), argIdentifier(argIdent), body(body)
	{
	}
	virtual ~LambdaFunction()
	{
		//std::cout << argIdentifier << " " << body;
		//delete argIdentifier;
		//DeleteExpression(body);
	}
};

struct Application : Expression
{
	Expression* lambda;
	Expression* argument;

	Application(Expression* lambda, Expression* argument)
		: Expression(false, ExpressionID::E_FUNCTION_APPLICATION), lambda(lambda), argument(argument)
	{
	}
	virtual ~Application()
	{
		std::cout << lambda << " " << argument;
		DeleteExpression(lambda);
		DeleteExpression(argument);
	}
};

struct Integer : Expression
{
	int i;
	Integer(int i)
		: Expression(true, ExpressionID::E_INTEGER), i(i)
	{
	}
	virtual ~Integer(){}
};

struct Boolean : Expression
{
	bool b;
	Boolean(bool b)
		: Expression(true, ExpressionID::E_BOOLEAN), b(b)
	{
	}
	virtual ~Boolean() {}
};

struct Operation : Expression
{
	Expression* left;
	Expression* right;
	Operator op;

	Operation(Expression* left, Expression* right, Operator op)
		: Expression(false, ExpressionID::E_OPERATION), left(left), right(right), op(op)
	{
	}
	virtual ~Operation()
	{
		//std::cout << "teste" << std::endl;
		DeleteExpression(left);
		DeleteExpression(right);
	}
};

struct Pointer : Expression
{
	int p;
	Pointer(int p)
		: Expression(true, ExpressionID::E_POINTER), p(p)
	{
	}
	virtual ~Pointer() {}
};

struct Error : Expression
{
	Error()
		: Expression(true, ExpressionID::E_ERROR)
	{
	}
};

struct UnaryIncrement : Expression
{
	Expression* i;
	UnaryIncrement(Expression* i)
		: Expression(false, ExpressionID::E_UNARY_INC), i(i)
	{
	}
	virtual ~UnaryIncrement()
	{
		DeleteExpression(i);
	}
};

struct UnaryDecrement : Expression
{
	Expression* i;
	UnaryDecrement(Expression* i)
		: Expression(false, ExpressionID::E_UNARY_DEC), i(i)
	{
	}
	virtual ~UnaryDecrement()
	{
		DeleteExpression(i);
	}
};

struct FunctionDeclaration : Expression
{
	VariableName* argIdentifier;
	Expression* body;
	std::string functionName;

	FunctionDeclaration(std::string functionName, VariableName* vn, Expression* body)
		: Expression(false, ExpressionID::E_FUNCTION_DECLARATION), argIdentifier(vn), body(body),
		functionName(functionName)
	{
	}
	virtual ~FunctionDeclaration()
	{
		delete argIdentifier;
		DeleteExpression(body);
	}
};

struct IfThenElse : Expression
{
	Expression* booleanTest;
	Expression* iftrue;
	Expression* iffalse;
	IfThenElse(Expression* booleanTest, Expression* iftrue, Expression* iffalse)
		: Expression(false, ExpressionID::E_IF_ELSE), booleanTest(booleanTest), iftrue(iftrue), iffalse(iffalse)
	{
	}
	virtual ~IfThenElse()
	{
		DeleteExpression(booleanTest);
		DeleteExpression(iftrue);
		DeleteExpression(iffalse);
	}
};

struct VariableDeclaration : Expression
{
	VariableName* name;
	Expression* value;
	VariableDeclaration(VariableName* name, Expression* value)
		: Expression(false, ExpressionID::E_VAR_DECLARATION), name(name), value(value)
	{
	}
	virtual ~VariableDeclaration()
	{
		delete name;
		DeleteExpression(value);
	}
};

struct Skip : Expression
{
	Skip()
		: Expression(true, ExpressionID::E_SKIP)
	{
	}
	virtual ~Skip() {}
};

struct Dereference : Expression
{
	VariableName* pointer;
	Dereference(VariableName* pointer)
		: Expression(false, ExpressionID::E_DEREFERENCE), pointer(pointer)
	{
	}
	virtual ~Dereference()
	{
		DeleteExpression(pointer);
	}
};

struct TryCatch : Expression
{
	Expression* toTry;
	Expression* toCatch;
	TryCatch(Expression* toTry, Expression* toCatch)
		: Expression(false, ExpressionID::E_TRY_CATCH), toTry(toTry), toCatch(toCatch)
	{
	}
	virtual ~TryCatch()
	{
		DeleteExpression(toTry);
		DeleteExpression(toCatch);
	}
};

struct Sequence : Expression
{
	Expression* left;
	Expression* right;
	Sequence(Expression* left, Expression* right)
		: Expression(false, ExpressionID::E_SEQUENCE), left(left), right(right)
	{
	}
	~Sequence()
	{
		DeleteExpression(left);
		DeleteExpression(right);
	}
};