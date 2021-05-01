/******************************************************************************
 test_CSharpStyler.cpp

	Test bash styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBCSharpStyler.h"
#include "cbTestUtil.h"
#include <jFileUtil.h>
#include <sstream>
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

	RunTest(st, styler, "./data/c#/test.cs", "./data/c#/styled.out");
}
