/******************************************************************************
 test_INIStyler.cpp

	Test INI styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBINIStyler.h"
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

	CBStylerBase* styler = CBINIStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/ini/test.ini", "./data/styler/ini/styled.out");
}
