#include <iostream>
#include "expression.h"
#include "smallstep.h"
#include "utils.h"

#define NL (std::cout << std::endl)

int main(int argc, char** argv)
{
	//	# Declaração de programa para calcular fatorial
	//
	//	fac(int:x)
	//	{
	//		if(x == 1)
	//			1
	//		else
	//			(fac, x - 1)
	//	}
	//	Valor esperado x!
	Expression* fac_program = new FunctionDeclaration(std::string("fac"), new VariableName(std::string("x")),
		new IfThenElse(
			new Operation(new VariableName("x"), new Integer(1), Operator::O_EQUALS),
			new Integer(1),
			new Application(new VariableName("fac"),
				new Operation(new VariableName("x"), new Integer(1), Operator::O_MINUS))
		)
	);
	//PrintProgram(fac_program); NL;

	//	# Declaração de programa para calcular (5 + 4) * (2 / 2)
	//
	//	(5 + 4) * (2 / 2)
	//
	//	Valor esperado: 18
	Expression* op_program = new Operation(
		new Operation(new Integer(5), new Integer(4), Operator::O_PLUS), 
		new Operation(new Integer(2), new Integer(1), Operator::O_DIV),
		Operator::O_MULT);
	//PrintProgram(op_program); NL;

	//	# Declaração de programa para teste do if
	//
	//	if (2 == 1)
	//	{
	//		5
	//	}
	//	else 
	//	{
	//		2 / 0
	//	}
	//
	//	Valor esperado: Error (divisao por zero)
	Expression* if_program = new IfThenElse(
		new Operation(new Integer(2), new Integer(1), Operator::O_EQUALS),
		new Integer(5),
		new Operation(new Integer(2), new Integer(0), Operator::O_DIV)
	);
	//PrintProgram(if_program); NL;

	//	# Declaração de programa para teste do try/catch
	//
	//	try 
	//	{
	//		1 / 0
	//	}
	//	catch
	//	{
	//		1 / 1
	//	}
	//
	//	Valor esperado: 1
	Expression* try_program = new TryCatch(
		new Operation(new Integer(1), new Integer(0), Operator::O_DIV),
		new Operation(new Integer(1), new Integer(1), Operator::O_DIV)
	);
	//PrintProgram(try_program); NL;

	// # Declaração de programa para teste do ++ e --
	//
	//	((3 + (4++ - 3)))--
	//
	//	Valor esperado: 4
	Expression* un_program = new UnaryDecrement(new Operation(
		new Integer(3),
		new Operation(
			new UnaryIncrement(new Integer(4)), 
			new Integer(3), 
			Operator::O_MINUS),
		Operator::O_PLUS
	));
	//PrintProgram(un_program); NL;

	// # Declaração de programa para teste de declaração de função
	//
	//	soma_um(x:T)
	//	{
	//		1 + 1
	//	}
	//	Valor esperado: skip
	Expression* decl_program = new FunctionDeclaration(
		std::string("soma_um"),
		new VariableName("x"),
		new Operation(new Integer(1), new Integer(1), Operator::O_PLUS)
	);
	//PrintProgram(decl_program); NL;

	// # Declaração de programa para teste da sequência de comandos
	//
	//	soma_um(x:T)
	//	{
	//		1
	//	};
	//	2 + 2
	//
	//	Valor esperado skip;2 + 2 => 4
	//	Expression* sq_program = new Sequence(
	//		new FunctionDeclaration(std::string("nome"), new VariableName("x"), new Integer(1)),
	//		new Operation(new Integer(2), new Integer(2), Operator::O_PLUS)
	//	);
	Expression* sq_program = new Sequence(
		new FunctionDeclaration(std::string("soma_um"), new VariableName("x"), new Integer(1)),
		new FunctionDeclaration(std::string("soma_um"), new VariableName("x"), new Integer(1))
	);
	//PrintProgram(sq_program);

	//	# Declaração de programa para teste de aplicação de função
	//
	//	(_(x:T1) T2 
	//	{ 
	//		if (x > 1)
	//		{
	//			x + 2
	//		}
	//		else
	//		{
	//			x - 1
	//		}
	//	}, 5)
	//
	//	Valor esperado: x + 2
	//	Expression* ap_program = new Application(
	//		new LambdaFunction(new VariableName("x"),
	//			new Operation(new VariableName("x"), new Integer(1), Operator::O_PLUS)),
	//		new Integer(5)
	//	);
	Expression* ap_program = 
		new Application(
			new LambdaFunction(
				new VariableName("x"),
				new IfThenElse(
					new Operation(new VariableName("x"), new Integer(1), Operator::O_GTHAN),
					new Operation(new VariableName("x"), new Integer(2), Operator::O_PLUS),
					new Operation(new VariableName("x"), new Integer(1), Operator::O_MINUS)
				)
			),
			new Integer(5)
		);
	PrintProgram(ap_program);



	// Small Step aplicação
	Expression* final_value = ap_program;
	while (!final_value->isValue)
	{
		final_value = SmallStep::Step(final_value);
		if (final_value != nullptr)
		{
			if (final_value->expID == ExpressionID::E_ERROR)
			{
				SmallStep::PrintError(SError::ERROR_RT);
				break;
			}
		}
		else
		{
			SmallStep::PrintError(SError::ERROR_SS);
			break;
		}
	}
	std::cout << "\n";
	PrintProgram(final_value);

	return std::cin.get();
}