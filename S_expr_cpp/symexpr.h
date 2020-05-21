#ifndef SYMEXPR_H
#define SYMEXPR_H

#include <stdio.h>
#include <stdlib.h>
#include "macros.h"

typedef struct Symbolic_expression
{
	short int op;
	struct Symbolic_expression* lhs;
	struct Symbolic_expression* rhs;
	int key;
}SYMEXPR, *PSYMEXPR;

PSYMEXPR new_symexpr(short int op, PSYMEXPR lhs, PSYMEXPR rhs, int key);
PSYMEXPR copy_symexpr(PSYMEXPR e);
PSYMEXPR derivative(PSYMEXPR e);
void print_symexpr(PSYMEXPR e);
void deallocate_symexpr(PSYMEXPR e);

//std::ostream& operator<<(std::ostream& os, const SYMEXPR& e);
//bool operator==(const SYMEXPR& lhs, const SYMEXPR& rhs);
//
//std::shared_ptr<SYMEXPR> derivative(std::shared_ptr<SYMEXPR> e);
//std::shared_ptr<SYMEXPR> simplify(std::shared_ptr<SYMEXPR> e);

#endif // !SYMEXPR_H

