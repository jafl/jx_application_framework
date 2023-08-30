/******************************************************************************
 test_JWebBrowser.cpp

	Program to test JWebBrowser class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JWebBrowser.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(ConvertVarNames)
{
	JString s("xterm \\' %f %g %y $f", JString::kNoCopy);
	JWebBrowser::ConvertVarNames(&s, "fgx");
	JAssertStringsEqual("xterm \\\\' $f $g %y \\$f", s);
}
