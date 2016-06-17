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

bool SmallStep::NameExistMemory(std::string& name, Expression** f_body)
{
	for (MemoryValue m : memory)
	{
		if (m.ptr_name.compare(name) == 0)
		{
			if (f_body != nullptr)
				*f_body = m.e;
			return true;	// equal strings
		}
	}
	return false;
}

LambdaFunction* SmallStep::LambdaFromDeclaration(FunctionDeclaration* fd)
{
	LambdaFunction* lf = new LambdaFunction(new VariableName(fd->argIdentifier->name), CopyExpression(fd->body));
	return lf;
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
		case ExpressionID::E_FUNCTION_APPLICATION: {
			Application* ap = (Application*)body;
			ap->argument = Substitution(param_name, ap->argument, argument);
			return body;
		} break;
		case ExpressionID::E_TRY_CATCH: {
			TryCatch* tc = (TryCatch*)body;
			tc->toCatch = Substitution(param_name, tc->toCatch, argument);
			tc->toTry = Substitution(param_name, tc->toTry, argument);
			return body;
		} break;
		case ExpressionID::E_UNARY_INC: {
			UnaryIncrement* ui = (UnaryIncrement*)body;
			ui->i = Substitution(param_name, ui->i, argument);
			return body;
		} break;
		case ExpressionID::E_UNARY_DEC: {
			UnaryDecrement* ud = (UnaryDecrement*)body;
			ud->i = Substitution(param_name, ud->i, argument);
		} break;
		case ExpressionID::E_FUNCTION_DECLARATION: {
			FunctionDeclaration* fd = (FunctionDeclaration*)body;
			fd->body = Substitution(param_name, fd->body, argument);
			return body;
		} break;
		case ExpressionID::E_SEQUENCE: {
			Sequence* sq = (Sequence*)body;
			sq->left = Substitution(param_name, sq->left, argument);
			sq->right = Substitution(param_name, sq->right, argument);
			return body;
		} break;
		case ExpressionID::E_VAR_DECLARATION: {
			VariableDeclaration* vd = (VariableDeclaration*)body;
			vd->value = Substitution(param_name, vd->value, argument);
			return body;
		} break;
		case ExpressionID::E_DEREFERENCE:
			return body; break;

		// VALUES
		case ExpressionID::E_LAMBDA_FUNCTION: {
			LambdaFunction* lf = (LambdaFunction*)body;

			// se nao iguais substitui
			if (lf->argIdentifier->name.compare(param_name->name))
				lf->body = Substitution(param_name, lf->body, argument);
			return body;
		} break;
		case ExpressionID::E_INTEGER:
		case ExpressionID::E_BOOLEAN:
		case ExpressionID::E_POINTER:
		case ExpressionID::E_ERROR:
		case ExpressionID::E_SKIP:
			return body;
			break;
		default: {
			std::cout << "Erro de substituicao no step de numero: " << step_number << std::endl;
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
		else
			return exp;
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

		if (!NameExistMemory(fd->functionName, nullptr))
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
		Dereference* dr = (Dereference*)ast;
		Expression* f_body;
		if (NameExistMemory(dr->pointer->name, &f_body))
		{
			if (f_body->expID == ExpressionID::E_VAR_DECLARATION)
			{
				VariableDeclaration* vd = (VariableDeclaration*)f_body;
				return CopyExpression(vd->value);
			}
		}

	} break;
	case ExpressionID::E_VAR_DECLARATION: {
		VariableDeclaration* vd = (VariableDeclaration*)ast;

		if (!vd->value->isValue)
		{
			vd->value = Step(vd->value);
			if (vd->expID == ExpressionID::E_ERROR)
				return new Error();
		}

		if (!NameExistMemory(vd->name->name, nullptr))
		{
			SmallStep::memory.push_back(MemoryValue(vd, new Pointer(memory.size()), vd->name->name));
			return new Skip();
		}
		else
		{
			std::string s("");
			s += "Nome de variavel declarado previamente: ";
			s += vd->name->name;
			global_error = s;
			return nullptr;
		}
	} break;
	case ExpressionID::E_VAR_NAME: {
		VariableName* vn = (VariableName*)ast;
		Expression* f_body;

		if (NameExistMemory(vn->name, &f_body))
		{
			if (f_body->expID == ExpressionID::E_FUNCTION_DECLARATION)
				return LambdaFromDeclaration((FunctionDeclaration*)f_body);
			else
				std::cout << "Variavel declarada sem dereferenciar." << std::endl;
			//else
			//	return CopyExpression(((VariableDeclaration*)f_body)->value);	// Permite uso de variaveis sem dereferencia
		}
		else
			std::cout << "Funcao "<< vn->name <<" nao declarada, Step[" << step_number << "]" << std::endl;

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

Expression* SmallStep::CopyExpression(Expression* exp)
{
	switch(exp->expID)
	{
		case ExpressionID::E_OPERATION: {
			Operation* op = (Operation*)exp;
			return new Operation(CopyExpression(op->left), CopyExpression(op->right), op->op);
		} break;
		case ExpressionID::E_IF_ELSE: {
			IfThenElse* ite = (IfThenElse*)exp;
			return new IfThenElse(CopyExpression(ite->booleanTest), 
				CopyExpression(ite->iftrue), CopyExpression(ite->iffalse));
		} break;
		case ExpressionID::E_TRY_CATCH: {
			TryCatch* tc = (TryCatch*)exp;
			return new TryCatch(CopyExpression(tc->toTry), CopyExpression(tc->toCatch));
		} break;
		case ExpressionID::E_UNARY_INC: {
			UnaryIncrement* ui = (UnaryIncrement*)exp;
			return new UnaryIncrement(CopyExpression(ui->i));
		} break;
		case ExpressionID::E_UNARY_DEC: {
			UnaryDecrement* ud = (UnaryDecrement*)exp;
			return new UnaryDecrement(CopyExpression(ud->i));
		} break;
		case ExpressionID::E_FUNCTION_DECLARATION: {
			FunctionDeclaration* fd = (FunctionDeclaration*)exp;
			return new FunctionDeclaration(fd->functionName,(VariableName*)fd->argIdentifier,
				CopyExpression(fd->body));
		} break;
		case ExpressionID::E_SEQUENCE: {
			Sequence* sq = (Sequence*)exp;
			return new Sequence(CopyExpression(sq->left), CopyExpression(sq->right));
		} break;
		case ExpressionID::E_FUNCTION_APPLICATION: {
			Application* ap = (Application*)exp;
			return new Application(CopyExpression(ap->lambda), CopyExpression(ap->argument));
		} break;
		case ExpressionID::E_DEREFERENCE: {
			Dereference* dr = (Dereference*)exp;
			return new Dereference((VariableName*)CopyExpression(dr->pointer));
		} break;
		case ExpressionID::E_VAR_DECLARATION: {
			VariableDeclaration* vd = (VariableDeclaration*)exp;
			return new VariableDeclaration((VariableName*)vd->name, CopyExpression(vd->value));
		} break;
		case ExpressionID::E_VAR_NAME: {
			VariableName* vn = (VariableName*)exp;
			return new VariableName(vn->name);
		} break;

			// VALUES
		case ExpressionID::E_INTEGER: {
			Integer* in = (Integer*)exp;
			return new Integer(in->i);
		} break;
		case ExpressionID::E_BOOLEAN: {
			Boolean* bo = (Boolean*)exp;
			return new Boolean(bo->b);
		} break;
		case ExpressionID::E_LAMBDA_FUNCTION: {
			LambdaFunction* lf = (LambdaFunction*)exp;
			return new LambdaFunction(new VariableName(lf->argIdentifier->name), CopyExpression(lf->body));
		} break;
		case ExpressionID::E_POINTER: {
			Pointer* pt = (Pointer*)exp;
			return new Pointer(pt->p);
		} break;
		case ExpressionID::E_ERROR:{
			Error* er = (Error*)exp;
			return new Error();
		} break;
		case ExpressionID::E_SKIP: {
			Skip* sk = (Skip*)exp;
			return new Skip();	
		} break;
		default: {
			std::cout << "Erro no step de numero: " << step_number << std::endl;
		} break;
	}
}

/*switch(exp->expID)
	{
		case ExpressionID::E_OPERATION: {
			Operation* op = (Operation*)exp;
		} break;
		case ExpressionID::E_IF_ELSE: {
			IfThenElse* ite = (IfThenElse*)exp;
		} break;
		case ExpressionID::E_TRY_CATCH: {
			TryCatch* tc = (TryCatch*)exp;
		} break;
		case ExpressionID::E_UNARY_INC: {
			UnaryIncrement* ui = (UnaryIncrement*)exp;
		} break;
		case ExpressionID::E_UNARY_DEC: {
			UnaryDecrement* ud = (UnaryDecrement*)exp;
		} break;
		case ExpressionID::E_FUNCTION_DECLARATION: {
			FunctionDeclaration* fd = (FunctionDeclaration*)exp;
		} break;
		case ExpressionID::E_SEQUENCE: {
			Sequence* sq = (Sequence*)exp;
		} break;
		case ExpressionID::E_FUNCTION_APPLICATION: {
			Application* ap = (Application*)exp;
		} break;
		case ExpressionID::E_DEREFERENCE: {
			Dereference* dr = (Dereference*)exp;
		} break;
		case ExpressionID::E_VAR_DECLARATION: {
			VariableDeclaration* vd = (VariableDeclaration*)exp;
		} break;
		case ExpressionID::E_VAR_NAME: {
			VariableName* vn = (VariableName*)exp;
		} break;

			// VALUES
		case ExpressionID::E_INTEGER: {
			Integer* in = (Integer*)exp;
		} break;
		case ExpressionID::E_BOOLEAN: {
			Boolean* bo = (Boolean*)exp;
		} break;
		case ExpressionID::E_LAMBDA_FUNCTION: {
			LambdaFunction* lf = (LambdaFunction*)exp;
		} break;
		case ExpressionID::E_POINTER: {
			Pointer* pt = (Pointer*)exp;
		} break;
		case ExpressionID::E_ERROR:{
			Error* er = (Error*)exp;
		} break;
		case ExpressionID::E_SKIP: {
			Skip* sk = (Skip*)exp;
		} break;
		default: {
			std::cout << "Erro no step de numero: " << step_number << std::endl;
		} break;
	}
*/