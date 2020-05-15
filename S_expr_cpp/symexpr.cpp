#include "symexpr.h"

std::ostream& operator<<(std::ostream& os, const SYMEXPR& e)
{
	switch (e.op)
	{
	case Const:
		os << e.val;
		return os;
	case Var:
		os << "x";
		return os;
	case E:
		os << "e";
		return os;
	case Add:
		os << *(e.lhs) << "+" << *(e.rhs);
		return os;
	case Sub:
		os << "(" << *(e.lhs) << ")"
			<< "-"
			<< "(" << *(e.rhs) << ")";
		return os;
	case Mul:
	{
		// Special cases when lhs or rhs is a constant s-expr with value -1
		if ((*(e.lhs)).val == -1)
		{
			os << "-"
				<< "(" << *(e.rhs) << ")";
			return os;
		}
		if ((*(e.rhs)).val == -1)
		{
			os << "-"
				<< "(" << *(e.lhs) << ")";
			return os;
		}
		os << "(" << *(e.lhs) << ")"
			<< "*"
			<< "(" << *(e.rhs) << ")";
		return os;
	}		
	case Div:
		os << "(" << *(e.lhs) << ")"
			<< "/"
			<< "(" << *(e.rhs) << ")";
		return os;
	case Sin:
		os << "sin(" << *(e.lhs) << ")";
		return os;
	case Cos:
		os << "cos(" << *(e.lhs) << ")";
		return os;
	case Tan:
		os << "tan(" << *(e.lhs) << ")";
		return os;
	case Cotan:
		os << "cotan(" << *(e.lhs) << ")";
		return os;
	case Log:
		os << "log(" << *(e.lhs) << ")";
		return os;
	case Exp:
		os << *(e.lhs) << "**"
			<< "(" << *(e.rhs) << ")";
		return os;
	default:
		return os;
	}
}

bool operator==(const SYMEXPR& lhs, const SYMEXPR& rhs)
{
	if (&lhs == nullptr)
	{
		if (&rhs == nullptr)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	if (&rhs == nullptr)
	{
		return false;
	}

	if (lhs.op != rhs.op)
	{
		return false;
	}

	if (lhs.op == Const && rhs.op == Const)
	{
		if (lhs.val == rhs.val)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if ((lhs.op == E && rhs.op == E) || (lhs.op == Var && rhs.op == Var))
	{
		return true;
	}

	return (*(lhs.lhs) == *(rhs.lhs)) && (*(lhs.rhs) == *(rhs.rhs));
}

std::shared_ptr<SYMEXPR> derivative(std::shared_ptr<SYMEXPR> e)
{
	switch (e->op)
	{
	case Const:
		return std::make_shared<SYMEXPR>(0);
	case Var:
		return std::make_shared<SYMEXPR>(1);
	case Add:
		return std::make_shared<SYMEXPR>(Add, derivative(e->lhs), derivative(e->rhs));
	case Sub:
		return std::make_shared<SYMEXPR>(Sub, derivative(e->lhs), derivative(e->rhs));
	case Mul:
		return std::make_shared<SYMEXPR>(
			Add,
			std::make_shared<SYMEXPR>(Mul, derivative(e->lhs), e->rhs),
			std::make_shared<SYMEXPR>(Mul, e->lhs, derivative(e->rhs)));
	case Div:
		// (f/g)' = (fg' - f'g)/(g**2)
		// e->lhs = numerator = f
		// e->rhs = denominator = g
		return std::make_shared<SYMEXPR>(
			Div,
			std::make_shared<SYMEXPR>(
				Sub,
				std::make_shared<SYMEXPR>(Mul, derivative(e->lhs), e->rhs),
				std::make_shared<SYMEXPR>(Mul, e->lhs, derivative(e->rhs))
				),
			std::make_shared<SYMEXPR>(Mul, e->rhs, e->rhs)
			);
	case Sin:
		// (sin(f))' = cos(f) * f'
		// e->lhs = f
		return std::make_shared<SYMEXPR>(
			Mul,
			std::make_shared<SYMEXPR>(Cos, e->lhs),
			derivative(e->lhs));
	case Cos:
		// (cos(f))' = (-1 * sin(f)) * f'
		// e->lhs = f
		return std::make_shared<SYMEXPR>(
			Mul,
			std::make_shared<SYMEXPR>(
				Mul,
				std::make_shared<SYMEXPR>(-1),
				std::make_shared<SYMEXPR>(Sin, e->lhs)
				),
			derivative(e->lhs)
			);
	case Log:
		// (log(f))' = f'/f
		// e->lhs = f
		return std::make_shared<SYMEXPR>(
			Div,
			derivative(e->lhs),
			e->lhs
			);
	case Exp:
		// (a ** f)' = ((a ** f) * f') * log(a)
		// e->lhs = a
		// e->rhs = f
		return std::make_shared<SYMEXPR>(
			Mul,
			std::make_shared<SYMEXPR>(
				Mul,
				std::make_shared<SYMEXPR>(Exp, e->lhs, e->rhs),
				derivative(e->rhs)
				),
			std::make_shared<SYMEXPR>(Log, e->lhs)
			);
	default:
		return nullptr;
	}
}

std::shared_ptr<SYMEXPR> simplify(std::shared_ptr<SYMEXPR> e)
{
	if (e->lhs)
	{
		e->lhs = simplify(e->lhs);
	}
	if (e->rhs)
	{
		e->rhs = simplify(e->rhs);
	}

	switch (e->op)
	{
	case Add:
	{
		if (e->lhs->op == Const)
		{
			if (e->lhs->val == 0)
			{
				return e->rhs;
			}
			if (e->rhs->op == Const)
			{
				return std::make_shared<SYMEXPR>(e->lhs->val + e->rhs->val);
			}
		}

		if (e->rhs->op == Const)
		{
			if (e->rhs->val == 0)
			{
				return e->lhs;
			}
		}

		return e;		
	}
	case Sub:
	{
		if (e->lhs->op == Const)
		{
			if (e->lhs->val == 0)
			{
				return std::make_shared<SYMEXPR>(Mul, std::make_shared<SYMEXPR>(-1), e->rhs);
			}
			if (e->rhs->op == Const)
			{
				return std::make_shared<SYMEXPR>(e->lhs->val - e->rhs->val);
			}
		}

		if (e->rhs->op == Const)
		{
			if (e->rhs->val == 0)
			{
				return e->lhs;
			}
		}

		return e;
	}
	case Mul:
	{
		if (e->lhs->op == Const)
		{
			if (e->lhs->val == 0)
			{
				// If lhs s-expr is 0, simply return it
				// 0 * x = 0
				return e->lhs;
			}
			if (e->lhs->val == 1)
			{
				// If it is 1, return the rhs s-expr
				// 1 * x = x
				return e->rhs;
			}
		}

		if (e->rhs->op == Const)
		{
			// Symmetrical case
			if (e->rhs->val == 0)
			{
				return e->rhs;
			}
			if (e->rhs->val == 1)
			{
				return e->lhs;
			}
		}


		if (e->lhs->op == Mul && e->lhs->lhs->op == Const)
		{
			// Push constant operands up one layer, so that they
			// are always at the top of chained "Mul" type s-exprs 
			std::shared_ptr<SYMEXPR> aux = e->rhs;
			e->rhs = e->lhs->lhs;
			e->lhs->lhs = aux;
		}
		else if (e->rhs->op == Mul && e->rhs->lhs->op == Const)
		{
			// Symmetrical case
			std::shared_ptr<SYMEXPR> aux = e->lhs;
			e->lhs = e->rhs->lhs;
			e->rhs->lhs = aux;
		}

		if (e->rhs->op == Const)
		{
			// Also make sure constant operand is on lhs of expression
			std::shared_ptr<SYMEXPR> aux = e->lhs;
			e->lhs = e->rhs;
			e->rhs = aux;
		} 
		return e;
	}
	case Div:
	{
		if (*(e->lhs) == *(e->rhs))
		{
			// If numerator = denominator, return 1
			return std::make_shared<SYMEXPR>(1);
		}

		if (e->rhs->op == Const)
		{
			if (e->rhs->val == 1)
			{
				// If denominator is 1, return numerator
				return e->lhs;
			}
			if (e->rhs->val == -1)
			{
				// If denominator is -1, change the s-expr type to a multiplication
				return std::make_shared<SYMEXPR>(Mul, std::make_shared<SYMEXPR>(-1), e->lhs);
			}
		}

		// Else, nothing else to be simplified
		return e;
	}
	case Exp:
	{
		if (e->rhs->op == Const)
		{
			if (e->rhs->val == 1)
			{
				// If exponent is 1, return base
				return e->lhs;
			}
			else
			{
				// If exponent is 0, return 1
				return std::make_shared<SYMEXPR>(1);
			}
		}

		return e;
	}
	case Log:
	{
		if (e->lhs->op == E)
		{
			// log(e) = 1
			return std::make_shared<SYMEXPR>(1);
		}
		if (e->lhs->op == Const && e->lhs->val == 1)
		{
			// log(1) = 0
			return std::make_shared<SYMEXPR>(0);
		}

		return e;
	}
	default:
		return e;
	}
}
