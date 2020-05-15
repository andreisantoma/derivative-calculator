#ifndef SYMEXPR_H
#define SYMEXPR_H

#include <iostream>


typedef enum{Add, Sub, Div, Mul,
			Sin, Cos, Tan, Cotan,
			Log, Exp,
			Const, Var, E}operation;

typedef struct Symbolic_expression
{
	operation op;
	std::shared_ptr<Symbolic_expression> lhs;
	std::shared_ptr<Symbolic_expression> rhs;
	int val;

	Symbolic_expression(operation op, std::shared_ptr<Symbolic_expression>lhs, std::shared_ptr<Symbolic_expression>rhs)
		: op(op), lhs(lhs), rhs(rhs), val(0)
	{
	}

	Symbolic_expression(operation op, std::shared_ptr<Symbolic_expression>lhs)
		: op(op), lhs(lhs), rhs(nullptr), val(0)
	{
	}

	Symbolic_expression(int val)
		: op(Const), lhs(nullptr), rhs(nullptr), val(val)
	{
	}

	Symbolic_expression(operation op)
		: op(op), lhs(nullptr), rhs(nullptr), val(0)
	{
	}

	~Symbolic_expression()
	{
		//std::cout << "BYE";
	}
}SYMEXPR;

std::ostream& operator<<(std::ostream& os, const SYMEXPR& e);
bool operator==(const SYMEXPR& lhs, const SYMEXPR& rhs);

std::shared_ptr<SYMEXPR> derivative(std::shared_ptr<SYMEXPR> e);
std::shared_ptr<SYMEXPR> simplify(std::shared_ptr<SYMEXPR> e);

#endif // !SYMEXPR_H

