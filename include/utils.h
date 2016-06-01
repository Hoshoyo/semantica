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

void assert(bool exp, const char* msg)
{
	if (exp == false)
		throw new Exception(msg);
}

void PrintOperator(Operator op)
{
	switch (op)
	{
		// Arithmetic
		case Operator::O_PLUS: {
			std::cout << "+";
		} break;
		case Operator::O_MINUS: {
			std::cout << "-";
		} break;
		case Operator::O_MULT: {
			std::cout << "*";
		} break;
		case Operator::O_DIV: {
			std::cout << "/";
		} break;

			// Comparation
		case Operator::O_EQUALS: {
			std::cout << "==";
		} break;
		case Operator::O_LTHAN: {
			std::cout << "<";
		} break;
		case Operator::O_GTHAN: {
			std::cout << ">";
		} break;
		case Operator::O_LETHAN: {
			std::cout << "<=";
		} break;
		case Operator::O_GETHAN: {
			std::cout << ">=";
		} break;
	}
}

void PrintProgram(Expression* e)
{
	try
	{
		switch (e->expID)
		{
		case ExpressionID::E_OPERATION: {
			assert(!e->isValue, "################# Operation nao e valor! ###############\n");
			Operation* op = (Operation*)e;

			// Print
			std::cout << "(";
			PrintProgram(op->left);
			std::cout << " ";
			PrintOperator(op->op);
			std::cout << " ";
			PrintProgram(op->right);
			std::cout << ")";
		} break;

		case ExpressionID::E_FUNCTION_APPLICATION: {
			assert(!e->isValue, "################# Function application nao e valor! ###############\n");
			Application* app = (Application*)e;

			// Print
			std::cout << "(";
			PrintProgram(app->lambda);
			std::cout << ", ";
			PrintProgram(app->argument);
			std::cout << ")";
		} break;

		case ExpressionID::E_BOOLEAN: {
			assert(e->isValue, "################# Boolean deve ser valor! ###############\n");
			Boolean* bp = (Boolean*)e;

			// Print
			if (bp->b)
				std::cout << "true";
			else
				std::cout << "false";
		} break;

		case ExpressionID::E_INTEGER: {
			assert(e->isValue, "################# Integer deve ser valor! ###############\n");
			Integer* ip = (Integer*)e;

			// Print
			std::cout << ip->i;
		} break;

		case ExpressionID::E_LAMBDA_FUNCTION: {
			assert(e->isValue, "################# Lambda deve ser valor! ###############\n");
			LambdaFunction* lf = (LambdaFunction*)e;

			// Print
			std::cout << "_(T1:";
			std::cout << lf->argIdentifier->name;
			std::cout << ") T2 \n{\n   ";
			PrintProgram(lf->body);
			std::cout << "\n}";
		} break;

		case ExpressionID::E_VAR_NAME: {
			assert(!e->isValue, "################# Variable name nao deve ser valor! ###############\n");
			VariableName* vn = (VariableName*)e;

			// Print
			std::cout << vn->name;
		} break;
		}
	}
	catch (Exception* e)
	{
		std::cout << e->GetMessage() << std::endl;
	}
}
