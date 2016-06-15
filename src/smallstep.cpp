#include "smallstep.h"

int SmallStep::step_number = 0;
std::string SmallStep::global_error;
std::vector<MemoryValue> SmallStep::memory;

void SmallStep::PrintError(SError error_type)
{
	switch (error_type)
	{
	case SError::ERROR_SS:
		std::cout << "Small step error, "; break;
	case SError::ERROR_RT:
		std::cout << "Runtime error, "; break;
	default:
		std::cout << "Unknown error, "; break;
	}
	std::cout << "Step[" << step_number << "] " << global_error << std::endl;
}

bool SmallStep::NameExistMemory(std::string& name)
{
	for (MemoryValue m : memory)
	{
		if (m.ptr_name.compare(name) == 0)
			return true;	// equal strings
	}
	return false;
}

Expression* SmallStep::Substitution(VariableName* param_name, Expression* body, Expression* argument)
{
	switch (body->expID)
	{
		case ExpressionID::E_VAR_NAME: {
			VariableName* vn = (VariableName*)body;

			if (vn->name.compare(param_name->name))
				return body;
			else
				return argument;
		} break;
		case ExpressionID::E_OPERATION: {
			Operation* op = (Operation*)body;
			op->left = Substitution(param_name, op->left, argument);
			op->right = Substitution(param_name, op->right, argument);
			return body;
		} break;
		case ExpressionID::E_IF_ELSE: {
			IfThenElse* ite = (IfThenElse*)body;
			ite->booleanTest = Substitution(param_name, ite->booleanTest, argument);
			ite->iftrue = Substitution(param_name, ite->iftrue, argument);
			ite->iffalse = Substitution(param_name, ite->iffalse, argument);
			return body;
		} break;
	}
	return body;
}

Expression* SmallStep::Step(Expression* ast)
{
	step_number++;
	switch (ast->expID)
	{
	// NON-VALUES
	case ExpressionID::E_OPERATION: {
		Operation* exp = (Operation*)ast;

		// Se lado esquerdo avança
		if (!exp->left->isValue)
		{
			exp->left = Step(exp->left);
			if (exp->left->expID == ExpressionID::E_ERROR)
				return new Error();
		}
		// Se lado direito avança
		if (!exp->right->isValue)
		{
			exp->right = Step(exp->right);
			if (exp->right->expID == ExpressionID::E_ERROR)
				return new Error();
		}

		if(exp->left->expID == ExpressionID::E_INTEGER)
		{
			Integer* newinteger = nullptr;
			Boolean* newboolean = nullptr;
			int l = ((Integer*)exp->left)->i;
			int r = ((Integer*)exp->right)->i;

			switch (exp->op)
			{
			case Operator::O_PLUS:
				return new Integer(l + r);
				break;
			case Operator::O_MINUS:
				return new Integer(l - r);
				break;
			case Operator::O_MULT:
				return new Integer(l * r);
				break;
			case Operator::O_DIV:
				// Exception division by 0
				if (r == 0)
				{
					global_error = std::string("Exception: divisao por zero");
					return new Error();
				}
				return new Integer(l / r);
				break;
			case Operator::O_EQUALS:
				return new Boolean(l == r);
				break;
			case Operator::O_GETHAN:
				return new Boolean(l >= r);
				break;
			case Operator::O_GTHAN:
				return new Boolean(l > r);
				break;
			case Operator::O_LETHAN:
				return new Boolean(l <= r);
				break;
			case Operator::O_LTHAN:
				return new Boolean(l < r);
				break;
			default:
				std::cout << "Operador nao reconhecido. Step numero: " << step_number << std::endl;
			}
			return nullptr;
		}
	} break;
	case ExpressionID::E_IF_ELSE: {
		IfThenElse* ite = (IfThenElse*)ast;
		
		// Se teste booleano avança
		if (!ite->booleanTest->isValue)
		{
			ite->booleanTest = SmallStep::Step(ite->booleanTest);
			if (ite->booleanTest->expID == ExpressionID::E_ERROR)
				return new Error();
		}

		bool ite_bool = ((Boolean*)ite->booleanTest)->b;
		if (ite_bool)
			return ite->iftrue;
		else
			return ite->iffalse;

	} break;
	case ExpressionID::E_TRY_CATCH: {
		TryCatch* tc = (TryCatch*)ast;

		// Se não é valor avança
		if (!tc->toTry->isValue)
			tc->toTry = SmallStep::Step(tc->toTry);

		// Se erro, substituir por catch
		if (tc->toTry->expID == ExpressionID::E_ERROR)
			return tc->toCatch;
		// Senão retorna valor do try
		else
			return tc->toTry;
	} break;
	case ExpressionID::E_UNARY_INC:
	case ExpressionID::E_UNARY_DEC: {
		UnaryDecrement* ud = (UnaryDecrement*)ast;

		// Se expressao avança
		if (!ud->isValue)
		{
			ud->i = SmallStep::Step(ud->i);
			if (ud->i->expID == ExpressionID::E_ERROR)
				return new Error();
		}

		if (ud->i->expID == ExpressionID::E_INTEGER)
		{
			int val = ((Integer*)ud->i)->i;
			if (ast->expID == ExpressionID::E_UNARY_INC)
				return new Integer(val + 1);
			else
				return new Integer(val - 1);
		}
		else
		{
			std::cout << "Erro, tipo inteiro esperado" << std::endl;
			return nullptr;
		}
	} break;
	case ExpressionID::E_FUNCTION_DECLARATION: {
		FunctionDeclaration* fd = (FunctionDeclaration*)ast;

		if (!NameExistMemory(fd->functionName))
		{
			SmallStep::memory.push_back(MemoryValue(fd, new Pointer(memory.size()), fd->functionName));
			return new Skip();
		}
		else
		{
			std::string s("");
			s += "Nome de funcao declarado previamente: ";
			s += fd->functionName;
			global_error = s;
			return nullptr;
		}
	} break;
	case ExpressionID::E_SEQUENCE: {
		Sequence* sq = (Sequence*)ast;

		// Se avança
		if (!sq->left->isValue)
		{
			sq->left = SmallStep::Step(sq->left);
			if (sq->left->expID == ExpressionID::E_ERROR)
				return new Error();
		}

		if (sq->left->expID == ExpressionID::E_SKIP)
			return sq->right;
		else
		{
			std::cout << "Erro de tipo, lado esquerdo da sequencia nao e do tipo unit" << std::endl;
			return nullptr;
		}
	} break;
	case ExpressionID::E_FUNCTION_APPLICATION: {
		Application* ap = (Application*)ast;
		
		// Avança esquerda(lambda)
		if (!ap->lambda->isValue)
		{
			ap->lambda = SmallStep::Step(ap->lambda);
			if (ap->lambda->expID == ExpressionID::E_ERROR)
				return new Error();
		}

		// Avança argumento
		if (!ap->argument->isValue)
		{
			ap->argument = SmallStep::Step(ap->argument);
			if (ap->argument->expID == ExpressionID::E_ERROR)
				return new Error();
		}

		return Substitution(((LambdaFunction*)ap->lambda)->argIdentifier, 
			((LambdaFunction*)ap->lambda)->body, 
			ap->argument);
	} break;
	case ExpressionID::E_DEREFERENCE: {
		// involve environment
	} break;
	case ExpressionID::E_VAR_DECLARATION: {
		// involve environment
	} break;
	case ExpressionID::E_VAR_NAME: {
		// involve environment
	} break;

	// VALUES
	case ExpressionID::E_INTEGER:
	case ExpressionID::E_BOOLEAN:
	case ExpressionID::E_LAMBDA_FUNCTION:
	case ExpressionID::E_POINTER:
	case ExpressionID::E_ERROR:
	case ExpressionID::E_SKIP: 
		return ast;
	break;
	default: {
		std::cout << "Erro no step de numero: " << step_number << std::endl;
	} break;
	}
	return nullptr;
}
