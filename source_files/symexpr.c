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

void print(PSYMEXPR e)
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
		print(e->lhs);
		printf("+");
		print(e->rhs);
		return;
	case SUB:
	{	
		short int lhs_op = e->lhs->op;
		short int rhs_op = e->rhs->op;
		print(e->lhs);
		printf("-");
		if (rhs_op == ADD || rhs_op == SUB || rhs_op == DIV)
		{
			printf("(");
			print(e->rhs);
			printf(")");
		}
		else
		{
			print(e->rhs);
		}		
		return;
	}
	case MUL:
	{		
		short int lhs_op = e->lhs->op;
		short int rhs_op = e->rhs->op;
		if (lhs_op == ADD || lhs_op == SUB || lhs_op == EXP || lhs_op == DIV)
		{
			// Only use brackets in some cases
			printf("(");
			print(e->lhs);
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
			print(e->lhs);
			printf("*");
		}

		if (rhs_op == ADD || rhs_op == SUB || rhs_op == DIV || rhs_op == EXP)
		{
			// Only use brackets in some cases
			printf("(");
			print(e->rhs);
			printf(")");			
		}
		else
		{
			print(e->rhs);
		}
		return;
	}		
	case DIV:
	{
		short int lhs_op = e->lhs->op;
		short int rhs_op = e->rhs->op;		
		if (lhs_op == CONST || lhs_op == VAR || lhs_op == E)
		{
			print(e->lhs);
		}
		else
		{
			printf("(");
			print(e->lhs);
			printf(")");
		}		
		printf("/");


		if (rhs_op == CONST || rhs_op == VAR || rhs_op == E)
		{
			print(e->rhs);
		}
		else
		{
			printf("(");
			print(e->rhs);
			printf(")");
		}

		return;
	}
	case SIN:
		printf("sin(");
		print(e->lhs);
		printf(")");
		return;
	case COS:
		printf("cos(");
		print(e->lhs);
		printf(")");
		return;
	case TAN:
		printf("tan(");
		print(e->lhs);
		printf(")");
		return;
	case COTAN:
		printf("cotan(");
		print(e->lhs);
		printf(")");
		return;
	case LOG:
		printf("log(");
		print(e->lhs);
		printf(")");
		return;
	case SQRT:
		printf("sqrt(");
		print(e->lhs);
		printf(")");
		return;
	case EXP:
	{
		short int lhs_op = e->lhs->op;
		short int rhs_op = e->rhs->op;
		if (lhs_op == CONST || lhs_op == E || lhs_op == VAR)
		{			
			print(e->lhs);
		}
		else
		{	
			// Only use brackets in some cases
			printf("(");
			print(e->lhs);
			printf(")");
		}

		printf("^");

		if (rhs_op == ADD || rhs_op == SUB || rhs_op == EXP || rhs_op == DIV || rhs_op == MUL || (rhs_op == CONST && e->rhs->key < 0))
		{
			// Only use brackets in some cases
			printf("(");
			print(e->rhs);
			printf(")");
		}
		else
		{
			print(e->rhs);
		}
		return;
	}
	default:
		return;		
	}
}

void deallocate(PSYMEXPR e)
{
	if (e == NULL)
	{
		return;
	}

	deallocate(e->lhs);
	deallocate(e->rhs);
	free(e);
	return;
}

PSYMEXPR copy(PSYMEXPR e)
{
	if (e == NULL)
	{
		return NULL;
	}

	PSYMEXPR s = new_symexpr(e->op, NULL, NULL, e->key);
	s->lhs = copy(e->lhs);
	s->rhs = copy(e->rhs);
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
			new_symexpr(MUL, derivative(e->lhs), copy(e->rhs), 0),
			new_symexpr(MUL, copy(e->lhs), derivative(e->rhs), 0),
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
				new_symexpr(MUL, derivative(e->lhs), copy(e->rhs), 0),
				new_symexpr(MUL, copy(e->lhs), derivative(e->rhs), 0),
				0),
			new_symexpr(MUL, copy(e->rhs), copy(e->rhs), 0),
			0
		);
	case SIN:
		// (sin(f))' = cos(f) * f'
		// e->lhs = f
		return new_symexpr(MUL, new_symexpr(COS, copy(e->lhs), NULL, 0), derivative(e->lhs), 0);
	case COS:
		// (cos(f))' = (-1 * sin(f)) * f'
		// e->lhs = f
		return new_symexpr(
			MUL,
			new_symexpr(CONST, NULL, NULL, -1),
			new_symexpr(MUL, new_symexpr(SIN, copy(e->lhs), NULL, 0), derivative(e->lhs), 0),
			0
		);
	case TAN:
		// (tan(f))' = f' / ((cos(f)) ^ 2)
		// e->lhs = f
		return new_symexpr(
			DIV,
			derivative(e->lhs),
			new_symexpr(EXP, new_symexpr(COS, copy(e->lhs), NULL, 0), new_symexpr(CONST, NULL, NULL, 2), 0),
			0
		);
	case COTAN:
		// (cotan(f))' = -f' / ((sin(f)) ^ 2)
		// e->lhs = f
		return new_symexpr(
			DIV,
			new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), derivative(e->lhs), 0),
			new_symexpr(EXP, new_symexpr(SIN, copy(e->lhs), NULL, 0), new_symexpr(CONST, NULL, NULL, 2), 0),
			0
		);
	case LOG:
		// (log(f))' = f'/f
		// e->lhs = f
		return new_symexpr(
			DIV,
			derivative(e->lhs),
			copy(e->lhs),
			0
		);
	case EXP:
	{
		if (e->rhs->op == CONST)
		{
			// (f ^ a)' = a * (f ^ (a - 1)) * f', if a is a number
			// e->lhs = f
			// e->rhs = a
			return new_symexpr(
				MUL,
				copy(e->rhs),
				new_symexpr(
					MUL,
					new_symexpr(EXP, copy(e->lhs), new_symexpr(CONST, NULL, NULL, e->rhs->key - 1), 0),
					derivative(e->lhs),
					0
				),
				0
			);
		}
		else
		{
			// (a ^ f)' = ((a ^ f) * f') * log(a), if a is a number
			// e->lhs = a
			// e->rhs = f
			return new_symexpr(
				MUL,
				new_symexpr(MUL, new_symexpr(EXP, copy(e->lhs), copy(e->rhs), 0), derivative(e->rhs), 0),				
				new_symexpr(LOG, copy(e->lhs), NULL, 0),
				0
			);
		}	
	}
	case SQRT:
		// (sqrt(f))' = f' / (2 * sqrt(f))
		// e->lhs = f
		return new_symexpr(
			DIV,
			derivative(e->lhs),
			new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, 2), new_symexpr(SQRT, copy(e->lhs), NULL, 0), 0),
			0
		);
	default:
		return NULL;
	}
}

bool are_equal(PSYMEXPR e1, PSYMEXPR e2)
{
	if (e1 == NULL)
	{
		if (e2 == NULL)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (e2 == NULL)
	{
		// e1 is not null
		return false;
	}

	if (e1->op != e2->op)
	{
		return false;
	}
	else
	{
		if (e1->op == CONST)
		{
			if (e1->key != e2->key)
			{
				return false;
			}
		}

		return are_equal(e1->lhs, e2->lhs) && are_equal(e1->rhs, e2->rhs);
	}
}

PSYMEXPR simplify(PSYMEXPR e)
{
	if (e == NULL)
	{
		return NULL;
	}
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
				// 0 + E1 = E1
				// Deallocate addition node
				// and return rhs
				PSYMEXPR aux = e->rhs;
				// make sure addition node rhs is set to NULL, otherwise E1 would be deallocated too
				e->rhs = NULL;
				deallocate(e);
				return aux;
			}
			if (e->rhs->op == CONST)
			{
				// Both lhs and rhs are constants
				// add their values up in one node and deallocate the addition node
				e->lhs->key += e->rhs->key;
				PSYMEXPR aux = e->lhs;
				e->lhs = NULL;
				deallocate(e);

				// Return the remanining node
				return aux;
			}
		}

		if (e->rhs->op == CONST)
		{
			if (e->rhs->key == 0)
			{
				// E1 + 0= E1
				// Deallocate addition node
				// and return lhs
				PSYMEXPR aux = e->lhs;
				// make sure addition node lhs is set to NULL, otherwise E1 would be deallocated too
				e->lhs = NULL;
				deallocate(e);
				return aux;
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
				// remove expr containing the constant -1 and push rhs of multiplication up one level
				// E1 + ((-1) * E2)) = E1 - E2

				PSYMEXPR aux = e->rhs; // Multiplication node containing (-1) * E2
				e->rhs = e->rhs->rhs;

				// Deallocate the multiplication node
				// make sure its rhs is set to NULL, otherwise E2 would be deallocated too
				aux->rhs = NULL;
				deallocate(aux);
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
		if (are_equal(e->lhs, e->rhs))
		{
			// E1 - E1 = 0
			// Deallocate subtraction node and return 0
			deallocate(e);
			return new_symexpr(CONST, NULL, NULL, 0);

		}

		if (e->lhs->op == CONST)
		{
			if (e->lhs->key == 0)
			{
				// 0 - E1 = (-1) * E1
				PSYMEXPR to_return = new_symexpr(MUL, new_symexpr(CONST, NULL, NULL, -1), e->rhs, 0);

				// Deallocate the subtraction node
				// make sure to remove its reference to E1 so that it doesn't get deallocated
				e->rhs = NULL;
				deallocate(e);
				return to_return;
			}
			if (e->rhs->op == CONST)
			{
				// Both lhs and rhs are constants
				// add their values up in one node and deallocate the subtraction node
				e->lhs->key += e->rhs->key;
				PSYMEXPR aux = e->lhs;
				e->lhs = NULL;
				deallocate(e);

				// Return remaining node
				return aux;
			}
		}

		if (e->rhs->op == CONST)
		{
			if (e->rhs->key == 0)
			{
				// Clean up subtraction node and return lhs of subtraction
				PSYMEXPR aux = e->lhs;
				e->lhs = NULL;
				deallocate(e);
				return aux;
			}
		}

		if ((e->rhs->op == MUL || e->rhs->op == DIV) && e->rhs->lhs->op == CONST && e->rhs->lhs->key < 0)
		{
			// E1 - ((-c) * E2)) = E1 + (c * E2)
			// Change expression type from subtraction to addition
			e->op = ADD;

			if (e->rhs->lhs->key == -1)
			{
				// If -c = -1, then c = 1, resulting in a multiplication by 1, which is redundant
				// remove expr containing the constant -1 and push rhs of multiplication up one level
				// E1 - ((-1) * E2)) = E1 + E2

				PSYMEXPR aux = e->rhs; // Multiplication node containing (-1) * E2
				e->rhs = e->rhs->rhs;

				// Deallocate the multiplication node
				// make sure its rhs is set to NULL, otherwise E2 would be deallocated too
				aux->rhs = NULL;
				deallocate(aux);
			}
			else
			{
				// Change sign of constant
				e->rhs->lhs->key *= -1;
			}			
		}

		return e;
	}
	case MUL:
	{
		if (e->lhs->op == CONST)
		{
			if (e->lhs->key == 0)
			{
				// 0 * E1 = 0
				// Deallocate the multiplication node
				// and return node containing constant 0

				// Save node containing 0
				PSYMEXPR aux = e->lhs;

				// Remove reference to node containing 0 
				// so that it doesn't get deallocated
				e->lhs = NULL;

				deallocate(e);
				return aux;
			}
			if (e->lhs->key == 1)
			{
				// 1 * E1 = E1
				// Deallocate the multiplication node
				// and return node containing E1

				// Save node containing E1
				PSYMEXPR aux = e->rhs;

				// Remove reference to node containing E1
				// so that it doesn't get deallocated
				e->rhs = NULL;

				deallocate(e);
				return aux;
			}
			if (e->rhs->op == CONST)
			{
				// Both lhs and rhs are constants
				// Multiply them into one node and deallocate the multiplication node
				e->lhs->key *= e->rhs->key;
				PSYMEXPR aux = e->lhs;
				e->lhs = NULL;
				deallocate(e);

				// Return remaining node
				return aux;
			}
			if (e->rhs->op == MUL && e->rhs->lhs->op == CONST)
			{
				e->rhs->lhs->key *= e->lhs->key;
				PSYMEXPR aux = e->rhs;
				e->rhs = NULL;
				deallocate(e);
				return aux;
			}
		}

		if (e->rhs->op == CONST)
		{
			// Symmetrical case
			if (e->rhs->key == 0)
			{
				PSYMEXPR aux = e->rhs;
				e->rhs = NULL;
				deallocate(e);
				return aux;
			}
			if (e->rhs->key == 1)
			{
				PSYMEXPR aux = e->lhs;
				e->lhs = NULL;
				deallocate(e);
				return aux;
			}
			if (e->rhs->op == CONST)
			{
				// Also make sure constant operand is always on lhs of expression
				PSYMEXPR aux = e->lhs;
				e->lhs = e->rhs;
				e->rhs = aux;
			}
		}


		if (e->lhs->op == MUL && e->lhs->lhs->op == CONST)
		{
			// Push constant operands up one layer, so that they
			// are always at the top of chained MUL type s-exprs 
			PSYMEXPR aux = e->rhs;
			e->rhs = e->lhs->lhs;
			e->lhs->lhs = aux;
		}
		else if (e->rhs->op == MUL && e->rhs->lhs->op == CONST)
		{
			// Symmetrical case
			PSYMEXPR aux = e->lhs;
			e->lhs = e->rhs->lhs;
			e->rhs->lhs = aux;
		}

		 
		return e;
	}
	case DIV:
	{
		if (e->lhs->op == CONST && e->rhs->op == CONST)
		{
			int d = gcd(e->lhs->key, e->rhs->key);
			e->lhs->key /= d;
			e->rhs->key /= d;			
		}
		if (e->lhs->op == MUL && e->lhs->lhs->op == CONST && e->rhs->op == CONST)
		{
			int d = gcd(e->lhs->lhs->key, e->rhs->key);
			e->lhs->lhs->key /= d;
			e->rhs->key /= d;
		}
		if (e->lhs->op == CONST && e->rhs->op == MUL && e->rhs->lhs->op == CONST)
		{
			int d = gcd(e->lhs->key, e->rhs->lhs->key);
			e->lhs->key /= d;
			e->rhs->lhs->key /= d;
		}
		if (e->lhs->op == MUL && e->lhs->lhs->op == CONST && e->rhs->op == MUL && e->rhs->lhs->op == CONST)
		{
			int d = gcd(e->lhs->lhs->key, e->rhs->lhs->key);
			e->lhs->lhs->key /= d;
			e->rhs->lhs->key /= d;
		}

		if (are_equal(e->lhs, e->rhs))
		{
			// E1 / E1 = 1
			// Deallocate division node and return 1
			deallocate(e);
			return new_symexpr(CONST, NULL, NULL, 1);			
		}

		if (e->lhs->op == CONST && e->lhs->key == 0)
		{
			// 0 / E1 = 0
			// Deallocate division node and return 1
			deallocate(e);
			return new_symexpr(CONST, NULL, NULL, 0);			
		}

		if (e->rhs->op == CONST)
		{
			if (e->rhs->key == 1)
			{
				// E1 / 1 =  E1
				// If denominator is 1, deallocate the division node and return the numerator
				PSYMEXPR aux = e->lhs;
				e->lhs = NULL;
				deallocate(e);
				return aux;
			}
			if (e->rhs->key == -1)
			{
				// E1 / (-1) = (-1) * E1
				// If denominator is -1, change the s-expr type to a multiplication
				e->op = MUL;

				// Also move -1 on lhs of expression
				PSYMEXPR aux = e->lhs;
				e->lhs = e->rhs;
				e->rhs = aux;
			}
		}

		// Else, nothing else to be simplified
		return e;
	}
	case EXP:
	{
		if (e->rhs->op == CONST)
		{
			if (e->rhs->key == 1)
			{
				// If exponent is 1, return base
				// E1 ^ 1 = E1
				PSYMEXPR aux = e->lhs;

				// Deallocate exponentiation node
				e->lhs = NULL;
				deallocate(e);
				return aux;
			}
			else if (e->rhs->key == 0)
			{
				// If exponent is 0, return 1
				// E1 ^ 0 = 1

				// Deallocate expenentiation node
				deallocate(e);

				// return 1
				return new_symexpr(CONST, NULL, NULL, 1);
			}
		}

		return e;
	}
	case LOG:
	{
		if (e->lhs->op == E)
		{
			// log(e) = 1
			// Deallocate logarithm node
			deallocate(e);

			// return 1
			return new_symexpr(CONST, NULL, NULL, 1);
		}
		if (e->lhs->op == CONST && e->lhs->key == 1)
		{
			// log(1) = 0
			// Deallocate logarithm node
			deallocate(e);

			// return 0
			return new_symexpr(CONST, NULL, NULL, 0);
		}

		return e;
	}
	default:
		return e;
	}
}

PSYMEXPR simplify_repeatedly(PSYMEXPR e, bool verbose)
{
	PSYMEXPR last = NULL;
	PSYMEXPR current = e;
	if (verbose)
	{
		printf("Current simplification iteration: "); print(current); printf("\n");
	}

	do
	{
		deallocate(last);
		last = current;
		current = simplify(copy(last));
		if (verbose)
		{
			printf("Current simplification iteration: "); print(current); printf("\n");
		}		
	}
	while(!are_equal(last, current));

	if (verbose)
	{
		printf("Last two iterations are identical. Expression can't be simplified anymore.\n");
	}
	deallocate(last);
	return current;	
}

int gcd(int a, int b)
{
	while (b != 0)
    {
        a %= b;
        a ^= b;
        b ^= a;
        a ^= b;
    }

    return a;
}
