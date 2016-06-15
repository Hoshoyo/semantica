#include "expression.h"

void DeleteExpression(Expression* ex)
{
	switch (ex->expID)
	{
	case ExpressionID::E_VAR_NAME: {
		delete ((VariableName*)ex);
	} break;
	case ExpressionID::E_INTEGER: {
		delete ((Integer*)ex);
	} break;
	case ExpressionID::E_BOOLEAN: {
		delete ((Boolean*)ex);
	} break;
	case ExpressionID::E_FUNCTION_APPLICATION: {
		delete ((Application*)ex);
	} break;
	case ExpressionID::E_LAMBDA_FUNCTION: {
		delete ((LambdaFunction*)ex);
	} break;
	case ExpressionID::E_OPERATION: {
		delete ((Operation*)ex);
	} break;
	case ExpressionID::E_POINTER: {
		delete ((Pointer*)ex);
	} break;
	case ExpressionID::E_ERROR: {
		delete ((Error*)ex);
	} break;
	case ExpressionID::E_DEREFERENCE: {
		delete ((Dereference*)ex);
	} break;
	case ExpressionID::E_FUNCTION_DECLARATION: {
		delete ((FunctionDeclaration*)ex);
	} break;
	case ExpressionID::E_IF_ELSE: {
		delete ((IfThenElse*)ex);
	} break;
	case ExpressionID::E_SKIP: {
		delete ((Skip*)ex);
	} break;
	case ExpressionID::E_TRY_CATCH: {
		delete ((TryCatch*)ex);
	} break;
	case ExpressionID::E_UNARY_DEC: {
		delete ((UnaryDecrement*)ex);
	} break;
	case ExpressionID::E_UNARY_INC: {
		delete ((UnaryIncrement*)ex);
	} break;
	case ExpressionID::E_VAR_DECLARATION: {
		delete ((VariableDeclaration*)ex);
	} break;
	default: {
		throw "Bad Deletion";
	} break;
	}
}