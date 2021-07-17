/******************************************************************************
 test_CStyler.cpp

	Test C styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBCStyler.h"
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

	CBStylerBase* styler = CBCStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/c/test.c", "./data/styler/c/styled.out");
}
