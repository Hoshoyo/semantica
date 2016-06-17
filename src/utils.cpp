#include "utils.h"

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
	if (e == nullptr)
	{
		std::cout << "Programa mal formado" << std::endl;
		return;
	}
	try
	{
		switch (e->expID)
		{
		case ExpressionID::E_OPERATION: {
			assert(!e->isValue, "################# Operation nao e valor! ###############\n");
			Operation* op = (Operation*)e;

			// Print
			PrintProgram(op->left);
			std::cout << " ";
			PrintOperator(op->op);
			std::cout << " ";
			PrintProgram(op->right);
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
			std::cout << "\n}\n";
		} break;

		case ExpressionID::E_VAR_NAME: {
			assert(!e->isValue, "################# Variable name nao deve ser valor! ###############\n");
			VariableName* vn = (VariableName*)e;

			// Print
			std::cout << vn->name;
		} break;
		case ExpressionID::E_FUNCTION_DECLARATION: {
			assert(!e->isValue, "################# Declaracao de funcao nao deve ser valor! ###############\n");
			FunctionDeclaration* fd = (FunctionDeclaration*)e;
			std::cout << fd->functionName << "(";
			std::cout << fd->argIdentifier->name;
			std::cout << ":T)" << std::endl;
			std::cout << "{" << std::endl;
			PrintProgram(fd->body);
			std::cout << "\n}\n";
		} break;
		case ExpressionID::E_IF_ELSE: {
			assert(!e->isValue, "################# If then else nao deve ser valor! ###############\n");
			IfThenElse* ite = (IfThenElse*)e;
			std::cout << "if (";
			PrintProgram(ite->booleanTest);
			std::cout << ")\n{\n\t";
			PrintProgram(ite->iftrue);
			std::cout << "\n}\nelse\n{\n\t";
			PrintProgram(ite->iffalse);
			std::cout << "\n}\n";
		} break;
		case ExpressionID::E_TRY_CATCH: {
			assert(!e->isValue, "################# Try catch nao deve ser valor! ###############\n");
			TryCatch* tc = (TryCatch*)e;
			std::cout << "try\n{\n\t";
			PrintProgram(tc->toTry);
			std::cout << "\n}\ncatch\n{\n\t";
			PrintProgram(tc->toCatch);
			std::cout << "\n}\n";
		} break;
		case ExpressionID::E_UNARY_DEC: {
			assert(!e->isValue, "################# Unary inc/dec nao deve ser valor! ###############\n");
			UnaryDecrement* ud = (UnaryDecrement*)e;
			std::cout << "(";
			PrintProgram(ud->i);
			std::cout << ")--";
		} break;
		case ExpressionID::E_UNARY_INC: {
			assert(!e->isValue, "################# Unary inc/dec nao deve ser valor! ###############\n");
			UnaryIncrement* ui = (UnaryIncrement*)e;
			std::cout << "(";
			PrintProgram(ui->i);
			std::cout << ")++";
		} break;
		case ExpressionID::E_ERROR: {
			std::cout << "Error" << std::endl;
		} break;
		case ExpressionID::E_SKIP: {
			std::cout << "skip";
		} break;
		case ExpressionID::E_SEQUENCE: {
			assert(!e->isValue, "################# Sequencia nao deve ser valor! ###############\n");
			Sequence* sq = (Sequence*)e;
			PrintProgram(sq->left);
			std::cout << ";\n";
			PrintProgram(sq->right);
		} break;
		case ExpressionID::E_DEREFERENCE: {
			assert(!e->isValue, "################# Dereferencia nao deve ser valor! ###############\n");
			Dereference* dr = (Dereference*)e;
			std::cout << "!";
			PrintProgram(dr->pointer);
		} break;

		default: {
			std::cout << "Expressao nao reconhecida" << std::endl;
		} break;
		}
	}
	catch (Exception* e)
	{
		std::cout << e->GetMessage() << std::endl;
	}
}
