/******************************************************************************
 dynamic_loader.cpp

	Dynamically loads object code from .so library and uses it.

	Written by Glenn Bach & John Lindal.

 ******************************************************************************/

#include <ace/DLL.h>
#include <jTypes.h>

using SetFn  = JFloat (*)(const JFloat, const JFloat, const JFloat);
using EvalFn = JFloat (*)(const JFloat);

int
main()
{
	ACE_DLL module("./dynamic_loaded.so");
	SetFn set   = (SetFn) module.symbol("Set");
	EvalFn eval = (EvalFn) module.symbol("Eval");
	set(1, 2, 3);
	std::cout << eval(10) << std::endl;
}
