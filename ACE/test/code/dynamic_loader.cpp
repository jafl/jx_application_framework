/******************************************************************************
 dynamic_loader.cpp

	Dynamically loads object code from .so library and uses it.

	Written by Glenn Bach & John Lindal.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <ace/DLL.h>
#include <jTypes.h>

typedef JFloat (SetFn)(const JFloat, const JFloat, const JFloat);
typedef JFloat (EvalFn)(const JFloat);

int
main()
{
	ACE_DLL module("./dynamic_loaded.so");
	SetFn* set   = (SetFn*) module.symbol("Set");
	EvalFn* eval = (EvalFn*) module.symbol("Eval");
	set(1, 2, 3);
	cout << eval(10) << endl;
}
