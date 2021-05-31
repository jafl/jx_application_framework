/******************************************************************************
 test_CSharpStyler.cpp

	Test bash styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBCSharpStyler.h"
#include "cbTestUtil.h"
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JStyledText* st = jnew JStyledText(kJFalse, kJFalse);
	assert( st != nullptr );

	CBStylerBase* styler = CBCSharpStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/c#/test.cs", "./data/styler/c#/styled.out");
}
