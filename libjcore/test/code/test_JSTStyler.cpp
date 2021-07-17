/******************************************************************************
 test_JSTStyler.cpp

	Program to test JSTStyler class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include "StyledText.h"
#include "TestStyler.h"
#include <JColorManager.h>
#include <JRegex.h>
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

	text.SetText(JString("føø yes nø zäp bar bäz", JString::kNoCopy));

	JRunArrayIterator<JFont> iter(text.GetStyles());

	JFont f;
	iter.MoveTo(kJIteratorStartBefore, 2);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 6);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetGreenColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 10);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetRedColor(), f.GetStyle().color);
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 13);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 16);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	bool wrapped;
	const JStringMatch m1 = text.SearchForward(
		JStyledText::TextIndex(1,1), JRegex("yes"), false, false, &wrapped);
	text.ReplaceMatch(m1, JString("yäy", JString::kNoCopy), nullptr, false);

	iter.MoveTo(kJIteratorStartBefore, 2);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 6);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 10);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetRedColor(), f.GetStyle().color);
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 13);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 16);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	text.Undo();

	iter.MoveTo(kJIteratorStartBefore, 2);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 6);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetGreenColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 10);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetRedColor(), f.GetStyle().color);
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 13);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().strike);

	iter.MoveTo(kJIteratorStartBefore, 16);
	iter.Next(&f);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().strike);
}
