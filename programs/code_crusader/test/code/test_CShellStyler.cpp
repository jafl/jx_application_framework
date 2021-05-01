/******************************************************************************
 test_CShellStyler.cpp

	Test bash styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBCShellStyler.h"
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

	CBStylerBase* styler = CBCShellStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/cshell/test.csh", "./data/cshell/styled.out");
}
