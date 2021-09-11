/******************************************************************************
 test_JavaStyler.cpp

	Test Java styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBJavaStyler.h"
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

	CBStylerBase* styler = CBJavaStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/java/test.java", "./data/styler/java/styled.out");
}
