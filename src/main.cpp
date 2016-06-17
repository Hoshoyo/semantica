#include <iostream>
#include "expression.h"
#include "smallstep.h"
#include "utils.h"
#include "memory.h"
#include <vld.h>

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
	//			(fac, x - 1) * x
	//	};
	//
	//	(fac, 5)
	//
	//	Valor esperado x!
	Expression* fac_program = 
		new Sequence(
			new FunctionDeclaration(
				std::string("fac"), 
				new VariableName(std::string("x")),
				new IfThenElse(
					new Operation(new VariableName("x"), new Integer(1), Operator::O_EQUALS),
					new Integer(1),
					new Operation(
						new Application(new VariableName("fac"),
							new Operation(new VariableName("x"), new Integer(1), Operator::O_MINUS)),
						new VariableName("x"),
						Operator::O_MULT
					)
				)
			),
			new Application(
				new VariableName("fac"),
				new Integer(5)
			)
		);
	PrintProgram(fac_program); NL;
	
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
	/*
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
	//PrintProgram(ap_program);
	
	//	# Declaração de programa para teste de declaração de variáveis
	//
	//	var T : y 2 + 2;
	//	(_(x:T1) T2 { x + !y }, 5)
	//
	//	Valor esperado: 9
	Expression* de_program =
		new Sequence(
			new VariableDeclaration(
				new VariableName("y"),
				new Operation(new Integer(2), new Integer(2), Operator::O_PLUS)
			),
			new Application(
				new LambdaFunction(
					new VariableName("x"),
					new Operation(new VariableName("x"), new Dereference(new VariableName("y")), Operator::O_PLUS)
				),
				new Integer(5)
			)
		);

	
	// Small Step aplicação
	Expression* final_value = de_program;
	while (!final_value->isValue)
	{
		final_value = SmallStep::Step(final_value);
		PrintProgram(final_value);
		std::cout << std::endl << std::endl;;
		//std::cin.get();
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
	std::cout << "Valor final do programa: ";
	PrintProgram(final_value);
	*/
		delete fac_program;
		delete op_program;
		delete if_program;
		delete try_program;
		delete un_program;
		delete decl_program;
		delete sq_program;
	//	delete ap_program;	 // problem deleting
	//	delete de_program;	 // problem deleting

	std::cout << GetTotalMemoryUsed() << std::endl;
	//ClearMemory();
	return std::cin.get();
}