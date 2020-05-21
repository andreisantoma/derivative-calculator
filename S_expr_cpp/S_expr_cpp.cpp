#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#include <iostream>
#include "symexpr.h"



int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	PSYMEXPR a = new_symexpr(
		SIN,
		new_symexpr(VAR, NULL, NULL, 0),
		NULL,
		0);

	PSYMEXPR b = derivative(a);

	print_symexpr(a);
	printf("\n");
	print_symexpr(b);
	deallocate_symexpr(a);
	deallocate_symexpr(b);
	return 0;
}