#include "../include/header.h"

int main()
{
	
	VariableName v(std::string("x"));
	Integer one(1);
	Operation op(&v, &one, Operator::PLUS);

	LambdaFunction lf(v, &op);

	Integer five(5);
	Application app(&lf, &five);

	return 0;
}