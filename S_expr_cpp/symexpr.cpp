#include "symexpr.h"

PSYMEXPR new_symexpr(short int op, PSYMEXPR lhs, PSYMEXPR rhs, int key)
{
	PSYMEXPR s = (PSYMEXPR)malloc(sizeof(SYMEXPR));
	s->op = op;
	s->lhs = lhs;
	s->rhs = rhs;
	s->key = key;

	return s;
}

void print_symexpr(PSYMEXPR e)
{
	if (e == NULL)
	{
		return;
	}
	switch (e->op)
	{
	case CONST:
		printf("%d", e->key);
		return;
	case VAR:
		printf("x");
		return;
	case E:
		printf("e");
		return;
	case ADD:
		print_symexpr(e->lhs);
		printf("+");
		print_symexpr(e->rhs);
		return;
	case SUB:
		print_symexpr(e->lhs);
		printf("-(");
		print_symexpr(e->rhs);
		printf(")");
		return;
	case MUL:
	{
		short int lhs_op = e->lhs->op;
		short int rhs_op = e->rhs->op;

		if (lhs_op == ADD || lhs_op == SUB || lhs_op == EXP)
		{
			// Only use brackets if lhs expr is an addition, subtraction or exponentiation
			printf("(");
			print_symexpr(e->lhs);
			printf(")*");
		}
		else if (lhs_op == CONST && e->lhs->key == -1)
		{
			// If lhs of multiplication is a constant -1, just print "-"
			// without any multiplication symbol
			printf("-");
		}
		else
		{
			print_symexpr(e->lhs);
			printf("*");
		}

		if (rhs_op == ADD || rhs_op == SUB)
		{
			// Only use brackets if lhs expr is an addition or subtraction
			printf("(");
			print_symexpr(e->rhs);
			printf(")");			
		}
		else
		{
			print_symexpr(e->rhs);
		}
		return;
	}		
	case DIV:
		printf("(");
		print_symexpr(e->lhs);
		printf(")/(");
		print_symexpr(e->rhs);
		printf(")");
		return;
	case SIN:
		printf("sin(");
		print_symexpr(e->lhs);
		printf(")");
		return;
	case COS:
		printf("cos(");
		print_symexpr(e->lhs);
		printf(")");
		return;
	case TAN:
		printf("tan(");
		print_symexpr(e->lhs);
		printf(")");
		return;
	case COTAN:
		printf("cotan(");
		print_symexpr(e->lhs);
		printf(")");
		return;
	case LOG:
		printf("log(");
		print_symexpr(e->lhs);
		printf(")");
		return;
	case EXP:
		print_symexpr(e->lhs);
		printf("**(");
		print_symexpr(e->rhs);
		printf(")");
		return;
	default:
		return;		
	}
}

void deallocate_symexpr(PSYMEXPR e)
{
	if (e == NULL)
	{
		return;
	}

	deallocate_symexpr(e->lhs);
	deallocate_symexpr(e->rhs);
	free(e);
	return;
}

PSYMEXPR copy_symexpr(PSYMEXPR e)
{
	if (e == NULL)
	{
		return NULL;
	}

	PSYMEXPR s = new_symexpr(e->op, NULL, NULL, e->key);
	s->lhs = copy_symexpr(e->lhs);
	s->rhs = copy_symexpr(e->rhs);
	return s;
}

PSYMEXPR derivative(PSYMEXPR e)
{
	switch (e->op)
	{
	case CONST:
	case E:
		return new_symexpr(CONST, NULL, NULL, 0);
	case VAR:
		return new_symexpr(CONST, NULL, NULL, 1);
	case ADD:
		return new_symexpr(ADD, derivative(e->lhs), derivative(e->rhs), 0);
	case SUB:
		return new_symexpr(SUB, derivative(e->lhs), derivative(e->rhs), 0);
	case MUL:
		// (fg)' = f'g + fg'
		// e->lhs = f
		// e->rhs = g
		return new_symexpr(
			ADD,
			new_symexpr(MUL, derivative(e->lhs), copy_symexpr(e->rhs), 0),
			new_symexpr(MUL, copy_symexpr(e->lhs), derivative(e->rhs), 0),
			0
		);
	case DIV:
		// (f/g)' = (f'g - fg')/(g**2)
		// e->lhs = f
		// e->rhs = g
		return new_symexpr(
			DIV,
			new_symexpr(
				SUB,
				new_symexpr(MUL, derivative(e->lhs), copy_symexpr(e->rhs), 0),
				new_symexpr(MUL, copy_symexpr(e->lhs), derivative(e->rhs), 0),
				0),
			new_symexpr(MUL, copy_symexpr(e->rhs), copy_symexpr(e->rhs), 0),
			0
		);
	case SIN:
		// (sin(f))' = cos(f) * f'
		// e->lhs = f
		return new_symexpr(MUL, new_symexpr(COS, copy_symexpr(e->lhs), NULL, 0), derivative(e->lhs), 0);
	case COS:
		// (cos(f))' = (-1 * sin(f)) * f'
		// e->lhs = f
		return new_symexpr(
			MUL,
			new_symexpr(CONST, NULL, NULL, -1),
			new_symexpr(MUL, new_symexpr(COS, copy_symexpr(e->lhs), NULL, 0), derivative(e->lhs), 0),
			0
		);
	case LOG:
		// (log(f))' = f'/f
		// e->lhs = f
		return new_symexpr(
			DIV,
			derivative(e->lhs),
			copy_symexpr(e->lhs),
			0
		);
	case EXP:
		// (a ** f)' = ((a ** f) * f') * log(a)
		// e->lhs = a
		// e->rhs = f
		return new_symexpr(
			MUL,
			new_symexpr(EXP, copy_symexpr(e->lhs), copy_symexpr(e->rhs), 0),
			new_symexpr(LOG, copy_symexpr(e->lhs), NULL, 0),
			0
		);
	default:
		return NULL;
	}
}

//bool operator==(const SYMEXPR& lhs, const SYMEXPR& rhs)
//{
//	if (&lhs == nullptr)
//	{
//		if (&rhs == nullptr)
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	if (&rhs == nullptr)
//	{
//		return false;
//	}
//
//	if (lhs.op != rhs.op)
//	{
//		return false;
//	}
//
//	if (lhs.op == Const && rhs.op == Const)
//	{
//		if (lhs.val == rhs.val)
//		{
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//
//	if ((lhs.op == E && rhs.op == E) || (lhs.op == Var && rhs.op == Var))
//	{
//		return true;
//	}
//
//	return (*(lhs.lhs) == *(rhs.lhs)) && (*(lhs.rhs) == *(rhs.rhs));
//}
//


PSYMEXPR simplify(PSYMEXPR e)
{
	if (e == NULL)
	{
		return NULL;
	}
	//if (e->lhs)
	//{
	//	e->lhs = simplify(e->lhs);
	//}
	//if (e->rhs)
	//{
	//	e->rhs = simplify(e->rhs);
	//}
	e->lhs = simplify(e->lhs);
	e->rhs = simplify(e->rhs);

	switch (e->op)
	{
	case ADD:
	{
		if (e->lhs->op == CONST)
		{
			if (e->lhs->key == 0)
			{
				// Clean up node containing 0 and return the other one
				deallocate_symexpr(e->lhs);
				return e->rhs;
			}
			if (e->rhs->op == CONST)
			{
				// Both lhs and rhs are constants
				// add their values up in one node and deallocate the other one
				e->lhs->key += e->rhs->key;
				deallocate_symexpr(e->rhs);

				// Return the remanining node
				return e->lhs;
			}
		}

		if (e->rhs->op == CONST)
		{
			if (e->rhs->key == 0)
			{
				// Clean up node containing 0 and return the other one
				deallocate_symexpr(e->rhs);
				return e->lhs;
			}
		}

		if ((e->rhs->op == MUL || e->rhs->op == DIV) && e->rhs->lhs->op == CONST && e->rhs->lhs->key < 0)
		{
			// E1 + ((-c) * E2)) = E1 - (c * E2)
			// Change expression type from addition to subtraction
			e->op = SUB;

			if (e->rhs->lhs->key == -1)
			{
				// If -c = -1, then c = 1, resulting in a multiplication by 1, which is redundant
				// Remove expr containing the constant -1 and push rhs of multiplication up one level
				// E1 + ((-1) * E2)) = E1 - E2

				PSYMEXPR aux = e->rhs; // Multiplication node containing (-1) * E2
				e->rhs = e->rhs->rhs;

				// Deallocate the multiplication node
				// Make sure its rhs is set to NULL, otherwise E2 would be deallocated
				aux->rhs = NULL;
				deallocate_symexpr(aux);
			}
			else
			{
				// Change sign of constant
				e->rhs->lhs->key *= -1;
			}			
		}

		return e;		
	}	
	case SUB:
	{
		// START WORKING FROM HERE
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
