/******************************************************************************
 test_JFontManager.cc

	Program to test JFontManager class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(DefaultFont)
{
	const JFont f = JFontManager::GetDefaultFont();
	JAssertStringsEqual(JGetDefaultFontName(), f.GetName());
	JAssertEqual(JGetDefaultFontSize(), f.GetSize());
}

JTEST(DefaultMonospaceFont)
{
	const JFont f = JFontManager::GetDefaultMonospaceFont();
	JAssertStringsEqual(JGetDefaultMonospaceFontName(), f.GetName());
	JAssertEqual(JGetDefaultMonospaceFontSize(), f.GetSize());
}
