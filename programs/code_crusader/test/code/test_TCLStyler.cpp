/******************************************************************************
 test_TCLStyler.cpp

	Test TCL styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBTCLStyler.h"
#include "cbTestUtil.h"
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JStyledText* st = jnew JStyledText(false, false);
	assert( st != nullptr );

	CBStylerBase* styler = CBTCLStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/tcl/test.tcl", "./data/styler/tcl/styled.out");
}
