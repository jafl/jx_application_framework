/******************************************************************************
 test_JFontManager.cpp

	Program to test JFontManager class.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

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
