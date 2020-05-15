#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


#include <iostream>
#include "symexpr.h"

#define X std::make_shared<SYMEXPR>(Var)
#define E std::make_shared<SYMEXPR>(E)


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//std::shared_ptr<SYMEXPR> a = std::make_shared<SYMEXPR>(
	//	Sin,
	//	std::make_shared<SYMEXPR>(
	//		Sin,
	//		std::make_shared<SYMEXPR>(
	//			Add,
	//			std::make_shared<SYMEXPR>(),
	//			std::make_shared<SYMEXPR>(3))
	//		)
	//);
	//
	//std::shared_ptr<SYMEXPR> b = derivative(a);
	//simplify(b);
	//std::cout << *b << std::endl;

	std::shared_ptr<SYMEXPR> a = std::make_shared<SYMEXPR>(
		Div,
		std::make_shared<SYMEXPR>(
			Add,
			X,
			std::make_shared<SYMEXPR>(3)
			),
		std::make_shared<SYMEXPR>(
			Add,
			std::make_shared<SYMEXPR>(
				Mul,
				X,
				std::make_shared<SYMEXPR>(
					Cos,
					X
					)
				),
			std::make_shared<SYMEXPR>(5)
			)
		);

	//std::shared_ptr<SYMEXPR> a = std::make_shared<SYMEXPR>(
	//	Log,
	//	std::make_shared<SYMEXPR>(Exp, E, X)
	//);


	std::shared_ptr<SYMEXPR> b = derivative(a);

	std::cout << "derivative of " << *a << std::endl << "is " << *b; 

	simplify(b);

	std::cout << std::endl << *b;

	return 0;
}