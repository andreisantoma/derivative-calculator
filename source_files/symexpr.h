#ifndef SYMEXPR_H
#define SYMEXPR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "macros.h"

typedef struct Symbolic_expression
{
	short int op;
	struct Symbolic_expression* lhs;
	struct Symbolic_expression* rhs;
	int key;
}SYMEXPR, *PSYMEXPR;

PSYMEXPR new_symexpr(short int op, PSYMEXPR lhs, PSYMEXPR rhs, int key);
PSYMEXPR copy(PSYMEXPR e);
PSYMEXPR derivative(PSYMEXPR e);
PSYMEXPR simplify(PSYMEXPR e);
PSYMEXPR simplify_repeatedly(PSYMEXPR e, bool verbose);
void print(PSYMEXPR e);
void deallocate(PSYMEXPR e);
bool are_equal(PSYMEXPR e1, PSYMEXPR e2);
int gcd(int a, int b);


#endif // !SYMEXPR_H

