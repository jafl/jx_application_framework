/******************************************************************************
 test_DStyler.cpp

	Test D styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBDStyler.h"
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

	CBStylerBase* styler = CBDStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/d/test.d", "./data/styler/d/styled.out");
}
