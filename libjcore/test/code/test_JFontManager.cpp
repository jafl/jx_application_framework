/******************************************************************************
 test_JFontManager.cpp

	Program to test JFontManager class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JFontManager.h"
#include "jGlobals.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(DefaultFont)
{
	const JFont f = JFontManager::GetDefaultFont();
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());
	JAssertEqual(JFontManager::GetDefaultFontSize(), f.GetSize());
}

JTEST(DefaultMonospaceFont)
{
	const JFont f = JFontManager::GetDefaultMonospaceFont();
	JAssertStringsEqual(JFontManager::GetDefaultMonospaceFontName(), f.GetName());
	JAssertEqual(JFontManager::GetDefaultMonospaceFontSize(), f.GetSize());
}
