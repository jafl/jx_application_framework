/******************************************************************************
 test_RubyStyler.cpp

	Test Ruby styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBRubyStyler.h"
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

	CBStylerBase* styler = CBRubyStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	RunTest(st, styler, "./data/styler/ruby/test.rb", "./data/styler/ruby/styled.out");
}
