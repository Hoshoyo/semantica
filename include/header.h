#pragma once
#include <iostream>
#include <string>

enum Operator
{
	PLUS,
	MINUS
};

struct Expression
{
	bool isValue;		// n | b | _(T1:x) T2 { e } | p | error
};

struct VariableName : Expression
{
	std::string name;
	VariableName(std::string& name)
		: name(name), isValue(false)
	{
	}
};

struct LambdaFunction : Expression
{
	VariableName argIdentifier;
	Expression* body;

	LambdaFunction(VariableName argIdent, Expression* body)
		: isValue(true), argIdentifier(argIdent), body(body)
	{
	}
};

struct Application : Expression
{
	Expression* lambda;
	Expression* argument;

	Application(Expression* lambda, Expression* argument)
		: isValue(false), lambda(lambda), argument(argument)
	{
	}
};

struct Operation : Expression
{
	Expression* left;
	Expression* right;
	Operator op;

	Operation(Expression* left, Expression* right, Operator op)
		: isValue(false), left(left), right(right), op(op)
	{
	}
};

struct Integer : Expression
{
	int i;
	Integer(int i)
		: isValue(true)
	{
	}
};