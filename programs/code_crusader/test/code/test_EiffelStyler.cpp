/******************************************************************************
 test_EiffelStyler.cpp

	Test Eiffel styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBEiffelStyler.h"
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

	CBStylerBase* styler = CBEiffelStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/eiffel/test.e", "./data/styler/eiffel/styled.out");
}
