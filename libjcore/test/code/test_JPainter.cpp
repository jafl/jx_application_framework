/******************************************************************************
 test_JPainter.cpp

	Program to test JPainter class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "TestPainter.h"
#include "TestFontManagerMissingGlyphs.h"
#include "jGlobals.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(GetStringWidth)
{
	TestFontManagerMissingGlyphs fontMgr;
	JFont f1 = fontMgr.GetFont(JString("foo", JString::kNoCopy)),
		  f2 = fontMgr.GetFont(JString("bar", JString::kNoCopy));

	TestPainter p(&fontMgr);

	p.SetFont(f1);
	JAssertEqual(21, p.GetStringWidth(JString("123", JString::kNoCopy)));
	JAssertEqual(7, p.GetStringWidth(JString("w", JString::kNoCopy)));
	JAssertEqual(7, p.GetStringWidth(JString("m", JString::kNoCopy)));
	JAssertEqual(14, p.GetStringWidth(JString("mw", JString::kNoCopy)));
	JAssertEqual(21, p.GetStringWidth(JString("2mw", JString::kNoCopy)));
	JAssertEqual(21, p.GetStringWidth(JString("mw3", JString::kNoCopy)));
	JAssertEqual(28, p.GetStringWidth(JString("2mw3", JString::kNoCopy)));
	JAssertEqual(21, p.GetStringWidth(JString("m7w", JString::kNoCopy)));
	JAssertEqual(28, p.GetStringWidth(JString("2m7w", JString::kNoCopy)));
	JAssertEqual(28, p.GetStringWidth(JString("m7w3", JString::kNoCopy)));
	JAssertEqual(35, p.GetStringWidth(JString("2m7w3", JString::kNoCopy)));
	JAssertEqual(63, p.GetStringWidth(JString("234m7w345", JString::kNoCopy)));

	p.SetFont(f2);
	JAssertEqual(30, p.GetStringWidth(JString("123", JString::kNoCopy)));
	JAssertEqual(7, p.GetStringWidth(JString("w", JString::kNoCopy)));
	JAssertEqual(7, p.GetStringWidth(JString("m", JString::kNoCopy)));
	JAssertEqual(14, p.GetStringWidth(JString("mw", JString::kNoCopy)));
	JAssertEqual(24, p.GetStringWidth(JString("2mw", JString::kNoCopy)));
	JAssertEqual(24, p.GetStringWidth(JString("mw3", JString::kNoCopy)));
	JAssertEqual(34, p.GetStringWidth(JString("2mw3", JString::kNoCopy)));
	JAssertEqual(24, p.GetStringWidth(JString("m7w", JString::kNoCopy)));
	JAssertEqual(34, p.GetStringWidth(JString("2m7w", JString::kNoCopy)));
	JAssertEqual(34, p.GetStringWidth(JString("m7w3", JString::kNoCopy)));
	JAssertEqual(44, p.GetStringWidth(JString("2m7w3", JString::kNoCopy)));
	JAssertEqual(84, p.GetStringWidth(JString("234m7w345", JString::kNoCopy)));
}
