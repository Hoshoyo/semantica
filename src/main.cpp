#include <iostream>
#include "expression.h"
#include "utils.h"
#include <vld.h>

int main(int argc, char** argv)
{
	//Operation* op0 = new Operation(new Integer(1), new Integer(2), Operator::O_MINUS);
	Operation* op2 = new Operation(new Integer(9), new Integer(1), Operator::O_EQUALS);
	//Operation* op3 = new Operation(new Integer(2), new Dereference(new VariableName(std::string("z"))), Operator::O_MULT);
	
	LambdaFunction* lf = new LambdaFunction(new VariableName(std::string("x")), new Integer(1));
	Application* app1 = new Application(lf, op2);
	
	//PrintProgram(op3);

	//delete op0;
	//delete op3;
	delete app1;

	return std::cin.get();
}