/******************************************************************************
 test_GoStyler.cpp

	Test Go styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBGoStyler.h"
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

	CBStylerBase* styler = CBGoStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/go/test.go", "./data/styler/go/styled.out");
}
