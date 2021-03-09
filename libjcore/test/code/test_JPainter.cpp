/******************************************************************************
 test_JPainter.cpp

	Program to test JPainter class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include "TestPainter.h"
#include "TestFontManagerMissingGlyphs.h"
#include <jGlobals.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(GetStringWidth)
{
	TestFontManagerMissingGlyphs fontMgr;
	JFont f1 = fontMgr.GetFont(JString("foo", 0, kJFalse)),
		  f2 = fontMgr.GetFont(JString("bar", 0, kJFalse));

	TestPainter p(&fontMgr);

	p.SetFont(f1);
	JAssertEqual(21, p.GetStringWidth(JString("123", 0, kJFalse)));
	JAssertEqual(7, p.GetStringWidth(JString("w", 0, kJFalse)));
	JAssertEqual(7, p.GetStringWidth(JString("m", 0, kJFalse)));
	JAssertEqual(14, p.GetStringWidth(JString("mw", 0, kJFalse)));
	JAssertEqual(21, p.GetStringWidth(JString("2mw", 0, kJFalse)));
	JAssertEqual(21, p.GetStringWidth(JString("mw3", 0, kJFalse)));
	JAssertEqual(28, p.GetStringWidth(JString("2mw3", 0, kJFalse)));
	JAssertEqual(21, p.GetStringWidth(JString("m7w", 0, kJFalse)));
	JAssertEqual(28, p.GetStringWidth(JString("2m7w", 0, kJFalse)));
	JAssertEqual(28, p.GetStringWidth(JString("m7w3", 0, kJFalse)));
	JAssertEqual(35, p.GetStringWidth(JString("2m7w3", 0, kJFalse)));
	JAssertEqual(63, p.GetStringWidth(JString("234m7w345", 0, kJFalse)));

	p.SetFont(f2);
	JAssertEqual(30, p.GetStringWidth(JString("123", 0, kJFalse)));
	JAssertEqual(7, p.GetStringWidth(JString("w", 0, kJFalse)));
	JAssertEqual(7, p.GetStringWidth(JString("m", 0, kJFalse)));
	JAssertEqual(14, p.GetStringWidth(JString("mw", 0, kJFalse)));
	JAssertEqual(24, p.GetStringWidth(JString("2mw", 0, kJFalse)));
	JAssertEqual(24, p.GetStringWidth(JString("mw3", 0, kJFalse)));
	JAssertEqual(34, p.GetStringWidth(JString("2mw3", 0, kJFalse)));
	JAssertEqual(24, p.GetStringWidth(JString("m7w", 0, kJFalse)));
	JAssertEqual(34, p.GetStringWidth(JString("2m7w", 0, kJFalse)));
	JAssertEqual(34, p.GetStringWidth(JString("m7w3", 0, kJFalse)));
	JAssertEqual(44, p.GetStringWidth(JString("2m7w3", 0, kJFalse)));
	JAssertEqual(84, p.GetStringWidth(JString("234m7w345", 0, kJFalse)));
}
