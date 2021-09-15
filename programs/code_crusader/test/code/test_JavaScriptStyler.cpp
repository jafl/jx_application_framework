/******************************************************************************
 test_JavaScriptStyler.cpp

	Test JavaScript styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBJavaScriptStyler.h"
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

	CBStylerBase* styler = CBJavaScriptStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/javascript/test.js", "./data/styler/javascript/styled.out");
}
