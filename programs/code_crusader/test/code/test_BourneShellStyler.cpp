/******************************************************************************
 test_BourneShellStyler.cpp

	Test bash styler.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBBourneShellStyler.h"
#include "cbTestUtil.h"
#include <jFileUtil.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JStyledText* st = jnew JStyledText(kJFalse, kJFalse);
	assert( st != nullptr );

	CBStylerBase* styler = CBBourneShellStyler::Instance();

	// empty
	UpdateStyles(st, styler);

	JString text;
	JReadFile(JString("./data/bash/1.sh"), &text);

	st->SetText(text);
	UpdateStyles(st, styler);
}
