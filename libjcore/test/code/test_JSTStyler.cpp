/******************************************************************************
 test_JSTStyler.cpp

	Program to test JSTStyler class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include "StyledText.h"
#include "TestStyler.h"
#include <JColorManager.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	StyledText text;
	TestStyler styler;
	text.SetStyler(&styler);

	text.SetText(JString("foo yes no zap bar baz", kJFalse));

	JFontStyle style = text.GetStyles().GetElement(2).GetStyle();
	JAssertEqual(JColorManager::GetBlackColor(), style.color);
	JAssertFalse(style.bold);
	JAssertFalse(style.strike);

	style = text.GetStyles().GetElement(6).GetStyle();
	JAssertEqual(JColorManager::GetGreenColor(), style.color);
	JAssertFalse(style.bold);
	JAssertFalse(style.strike);

	style = text.GetStyles().GetElement(10).GetStyle();
	JAssertEqual(JColorManager::GetRedColor(), style.color);
	JAssertTrue(style.bold);
	JAssertFalse(style.strike);

	style = text.GetStyles().GetElement(13).GetStyle();
	JAssertEqual(JColorManager::GetBlackColor(), style.color);
	JAssertFalse(style.bold);
	JAssertTrue(style.strike);

	style = text.GetStyles().GetElement(16).GetStyle();
	JAssertEqual(JColorManager::GetBlackColor(), style.color);
	JAssertFalse(style.bold);
	JAssertFalse(style.strike);
}
