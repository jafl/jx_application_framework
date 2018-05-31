/******************************************************************************
 test_JTextEditor.cc

	Program to test JTextEditor class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include "TextEditor.h"
#include "StyledText.h"
#include <JRegex.h>
#include <JInterpolate.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

typedef JStyledText::TextIndex TextIndex;
typedef JStyledText::TextCount TextCount;
typedef JStyledText::TextRange TextRange;

int main()
{
	return JTestManager::Execute();
}

JTEST(LayoutBreakCROnly)
{
	StyledText text;
	text.SetText(JString(
		"Four sc" "\xC3\xB8" "re and seven years ago\n"
/*32*/	"our fathers brought forth on this continent,\n"
/*77*/	"a new nation,\n"
/*91*/	"conceived in Liberty,\n"
/*113*/	"and dedicated to the proposition that all men are created equal.\n", 0, kJFalse));

	TextEditor te(&text, kJTrue, 50);

	std::cout << "=====" << std::endl;
	te.Draw();
	std::cout << "=====" << std::endl;

	JAssertEqual(5, te.GetLineCount());
	JAssertEqual(65*7 + 12, te.GetWidth());
	JAssertEqual(6*7, te.GetHeight());
	JAssertEqual(1, te.GetLineForChar(8));
	JAssertEqual(2, te.GetLineForChar(50));
	JAssertEqual(21, te.GetLineTop(4));
	JAssertEqual(7, te.GetLineHeight(4));
	JAssertEqual(3, te.CRLineIndexToVisualLineIndex(3));
	JAssertEqual(4, te.VisualLineIndexToCRLineIndex(4));
	JAssertEqual(8, te.GetColumnForChar(8));
	JAssertEqual(5, te.GetColumnForChar(36));

	JAssertEqual(1, te.GetLineCharStart(1));
	JAssertEqual(31, te.GetLineCharLength(1));
	JAssertEqual(31, te.GetLineCharEnd(1));

	JAssertEqual(32, te.GetLineCharStart(2));
	JAssertEqual(45, te.GetLineCharLength(2));
	JAssertEqual(76, te.GetLineCharEnd(2));

	JAssertEqual(77, te.GetLineCharStart(3));
	JAssertEqual(14, te.GetLineCharLength(3));
	JAssertEqual(90, te.GetLineCharEnd(3));

	JAssertEqual(91, te.GetLineCharStart(4));
	JAssertEqual(22, te.GetLineCharLength(4));
	JAssertEqual(112, te.GetLineCharEnd(4));

	JAssertEqual(113, te.GetLineCharStart(5));

	JAssertEqual(10, te.GetCharLeft(1));
	JAssertEqual(59, te.GetCharLeft(8));
	JAssertEqual(66, te.GetCharRight(8));
	JAssertEqual(66, te.GetCharLeft(9));

	JIndex charIndex;
	JCharacterRange charRange;

	te.GoToLine(100);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(text.GetText().GetCharacterCount()+1, charIndex);

	te.GoToLine(4);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(91, charIndex);

	te.GoToColumn(4, 10);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(100, charIndex);

	te.GoToBeginningOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(91, charIndex);

	te.GoToEndOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(112, charIndex);

	te.SelectLine(2);
	JAssertFalse(te.GetCaretLocation(&charIndex));
	JAssertTrue(te.GetSelection(&charRange));
	JAssertEqual(32, charRange.first);

	te.GoToBeginningOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(32, charIndex);

	te.SelectLine(2);
	te.GoToEndOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(76, charIndex);

	JAssertEqual(1, te.CalcCaretCharLocation(JPoint(0,0)));
	JAssertEqual(1, te.CalcCaretCharLocation(JPoint(13,6)));
	JAssertEqual(2, te.CalcCaretCharLocation(JPoint(14,6)));
	JAssertEqual(8, te.CalcCaretCharLocation(JPoint(62,3)));
	JAssertEqual(9, te.CalcCaretCharLocation(JPoint(63,3)));
	JAssertEqual(32, te.CalcCaretCharLocation(JPoint(13,7)));
	JAssertEqual(33, te.CalcCaretCharLocation(JPoint(14,9)));

	te.SelectLine(2);
	JAssertFalse(te.TestPointInSelection(JPoint(13,6)));
	JAssertTrue(te.TestPointInSelection(JPoint(13,7)));

	te.SetCaretLocation(8);
	JAssertFalse(te.TestPointInSelection(JPoint(13,7)));

	te.TestMoveCaretVert(+2);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(84, charIndex);

	te.TestMoveCaretVert(-1);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(39, charIndex);
}

void
TestLayoutBreakWidth
	(
	JStyledText&		text,
	TextEditor&			te,
	const JCoordinate	width
	)
{
	te.SetBoundsWidth(width);

	std::cout << "=====" << std::endl;
	te.Draw();
	std::cout << "=====" << std::endl;

	JAssertEqual(30, te.GetLineCount());
	JAssertEqual(width, te.GetWidth());
	JAssertEqual(31*7, te.GetHeight());
	JAssertEqual(2, te.GetLineForChar(8));
	JAssertEqual(6, te.GetLineForChar(28));
	JAssertEqual(6, te.GetLineForChar(32));
	JAssertEqual(8, te.GetLineForChar(50));
	JAssertEqual(21, te.GetLineTop(4));
	JAssertEqual(7, te.GetLineHeight(4));
	JAssertEqual(1, te.CRLineIndexToVisualLineIndex(1));
	JAssertEqual(7, te.CRLineIndexToVisualLineIndex(2));
	JAssertEqual(14, te.CRLineIndexToVisualLineIndex(3));
	JAssertEqual(16, te.CRLineIndexToVisualLineIndex(4));
	JAssertEqual(20, te.CRLineIndexToVisualLineIndex(5));
	JAssertEqual(5, te.VisualLineIndexToCRLineIndex(20));
	JAssertEqual(3, te.GetColumnForChar(8));
	JAssertEqual(1, te.GetColumnForChar(36));

	JAssertEqual(1, te.GetLineCharStart(1));
	JAssertEqual(5, te.GetLineCharLength(1));
	JAssertEqual(5, te.GetLineCharEnd(1));

	JAssertEqual(6, te.GetLineCharStart(2));
	JAssertEqual(12, te.GetLineCharStart(3));
	JAssertEqual(16, te.GetLineCharStart(4));
	JAssertEqual(22, te.GetLineCharStart(5));
	JAssertEqual(28, te.GetLineCharStart(6));
	JAssertEqual(32, te.GetLineCharStart(7));
	JAssertEqual(36, te.GetLineCharStart(8));
	JAssertEqual(44, te.GetLineCharStart(9));
	JAssertEqual(52, te.GetLineCharStart(10));
	JAssertEqual(58, te.GetLineCharStart(11));
	JAssertEqual(66, te.GetLineCharStart(12));
	JAssertEqual(1, te.GetLineCharStart(13));
	JAssertEqual(1, te.GetLineCharStart(14));
	JAssertEqual(1, te.GetLineCharStart(15));
	JAssertEqual(1, te.GetLineCharStart(16));
	JAssertEqual(1, te.GetLineCharStart(17));
	JAssertEqual(1, te.GetLineCharStart(18));
	JAssertEqual(1, te.GetLineCharStart(19));
	JAssertEqual(1, te.GetLineCharStart(20));
	JAssertEqual(1, te.GetLineCharStart(21));
	JAssertEqual(1, te.GetLineCharStart(22));
	JAssertEqual(1, te.GetLineCharStart(23));
	JAssertEqual(1, te.GetLineCharStart(24));
	JAssertEqual(1, te.GetLineCharStart(25));
/*
	JAssertEqual(32, te.GetLineCharStart(2));
	JAssertEqual(45, te.GetLineCharLength(2));
	JAssertEqual(76, te.GetLineCharEnd(2));

	JAssertEqual(77, te.GetLineCharStart(3));
	JAssertEqual(14, te.GetLineCharLength(3));
	JAssertEqual(90, te.GetLineCharEnd(3));

	JAssertEqual(91, te.GetLineCharStart(4));
	JAssertEqual(22, te.GetLineCharLength(4));
	JAssertEqual(112, te.GetLineCharEnd(4));

	JAssertEqual(113, te.GetLineCharStart(5));

	JAssertEqual(10, te.GetCharLeft(1));
	JAssertEqual(59, te.GetCharLeft(8));
	JAssertEqual(66, te.GetCharRight(8));
	JAssertEqual(66, te.GetCharLeft(9));

	JIndex charIndex;
	JCharacterRange charRange;

	te.GoToLine(100);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(text.GetText().GetCharacterCount()+1, charIndex);

	te.GoToLine(4);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(91, charIndex);

	te.GoToColumn(4, 10);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(100, charIndex);

	te.GoToBeginningOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(91, charIndex);

	te.GoToEndOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(112, charIndex);

	te.SelectLine(2);
	JAssertFalse(te.GetCaretLocation(&charIndex));
	JAssertTrue(te.GetSelection(&charRange));
	JAssertEqual(32, charRange.first);

	te.GoToBeginningOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(32, charIndex);

	te.SelectLine(2);
	te.GoToEndOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(76, charIndex);

	JAssertEqual(1, te.CalcCaretCharLocation(JPoint(0,0)));
	JAssertEqual(1, te.CalcCaretCharLocation(JPoint(13,6)));
	JAssertEqual(2, te.CalcCaretCharLocation(JPoint(14,6)));
	JAssertEqual(8, te.CalcCaretCharLocation(JPoint(62,3)));
	JAssertEqual(9, te.CalcCaretCharLocation(JPoint(63,3)));
	JAssertEqual(32, te.CalcCaretCharLocation(JPoint(13,7)));
	JAssertEqual(33, te.CalcCaretCharLocation(JPoint(14,9)));

	te.SelectLine(2);
	JAssertFalse(te.TestPointInSelection(JPoint(13,6)));
	JAssertTrue(te.TestPointInSelection(JPoint(13,7)));

	te.SetCaretLocation(8);
	JAssertFalse(te.TestPointInSelection(JPoint(13,7)));

	te.TestMoveCaretVert(+2);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(84, charIndex);

	te.TestMoveCaretVert(-1);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(39, charIndex);
*/
}

JTEST(LayoutBreakWidth)
{
	StyledText text;
	text.SetText(JString(
		"Four sc" "\xC3\xB8" "re and seven years ago\n"
		"our fathers brought forth on this continent,\n"
		"a new nation,\n"
		"conceived in Liberty,\n"
		"and dedicated to the proposition that all men are created equal.\n", 0, kJFalse));

	TextEditor te(&text, kJFalse, 100);

//	TestLayoutBreakWidth(text, te, 12 + 49);
//	TestLayoutBreakWidth(text, te, 12 + 52);
//	TestLayoutBreakWidth(text, te, 12 + 55);
}

JTEST(GetCmdStatus)
{
}

JTEST(MouseActions)
{
	// TEGetDoubleClickSelection
}

JTEST(KeyPress)
{
}

JTEST(ReplaceSelection)	// after SearchTextBackward
{
}

JTEST(SearchTextForward)
{
	StyledText text;
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", 0, kJFalse));

	TextEditor te(&text, kJTrue, 50);

	// entire word, no wrapping

	JBoolean wrapped;
	JIndex caretIndex;
	const JStringMatch m1 = te.SearchForward(JRegex("sc" "\xC3\xB8" "re"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(te.HasSelection());	// caret still at beginning
	JAssertTrue(te.GetCaretLocation(&caretIndex));
	JAssertEqual(1, caretIndex);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	JString s;
	JCharacterRange r;
	const JStringMatch m2 = te.SearchForward(JRegex("sc" "\xC3\xB8" "re"), kJFalse, kJFalse, &wrapped);
	JAssertStringsEqual("sc" "\xC3\xB8" "re", m2.GetString());
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(5, 9), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("sc" "\xC3\xB8" "re", s);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m3 = te.SearchForward(JRegex("\xC3\xB8" "re and"), kJFalse, kJFalse, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(5, 9), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("sc" "\xC3\xB8" "re", s);
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = te.SearchForward(JRegex("\xC3\xB8" "re and"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("\xC3\xB8" "re and", m4.GetString());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(7, 13), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("\xC3\xB8" "re and", s);
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = te.SearchForward(JRegex("year"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(7, 13), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("\xC3\xB8" "re and", s);
	JAssertFalse(wrapped);

	const JStringMatch m7 = te.SearchForward(JRegex("years"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("years", m7.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// last characters

	te.SetCaretLocation(te.GetText()->GetText().GetCharacterCount() - 2);
	JAssertFalse(te.HasSelection());
	const JStringMatch m5 = te.SearchForward(JRegex("\\.\\.\\."), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("...", m5.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("...", s);
	JAssertFalse(wrapped);

	const JStringMatch m8 = te.SelectionMatches(JRegex("\\.\\.\\."), kJFalse);
	JAssertFalse(m8.IsEmpty());
	JAssertStringsEqual("...", m8.GetString());
}

JTEST(SearchTextBackward)
{
	StyledText text;
	text.SetText(JString("Fourscore and seve" "\xC3\xB1" " years ago...", 0, kJFalse));

	TextEditor te(&text, kJFalse, 50);
	te.SetCaretLocation(text.GetText().GetCharacterCount() + 1);

	// entire word, no wrapping

	JBoolean wrapped;
	const JStringMatch m1 = te.SearchBackward(JRegex("year"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(te.HasSelection());	// caret still at end
	JAssertFalse(wrapped);

	JString s;
	JCharacterRange r;
	const JStringMatch m2 = te.SearchBackward(JRegex("years"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("years", m2.GetString());
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(21, 25), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m3 = te.SearchBackward(JRegex("e" "\xC3\xB1" " ye"), kJFalse, kJFalse, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(21, 25), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = te.SearchBackward(JRegex("e" "\xC3\xB1" " ye"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("e" "\xC3\xB1" " ye", m4.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("e" "\xC3\xB1" " ye", s);
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = te.SearchBackward(JRegex("an"), kJTrue, kJFalse, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertTrue(te.GetSelection(&s));		// selection maintained
	JAssertStringsEqual("e" "\xC3\xB1" " ye", s);
	JAssertFalse(wrapped);

	const JStringMatch m7 = te.SearchBackward(JRegex("and"), kJTrue, kJFalse, &wrapped);
	JAssertStringsEqual("and", m7.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("and", s);
	JAssertFalse(wrapped);

	// first characters

	te.SetCaretLocation(5);
	JAssertFalse(te.HasSelection());
	const JStringMatch m5 = te.SearchBackward(JRegex("Four"), kJFalse, kJTrue, &wrapped);
	JAssertStringsEqual("Four", m5.GetString());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 4), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("Four", s);
	JAssertFalse(wrapped);
}

JTEST(ReplaceAll)
{
	StyledText text;
	text.SetText(JString("Fourscore and seven years ago...", 0, kJFalse));
	const JSize charCount = text.GetText().GetCharacterCount();

	TextEditor te(&text, kJTrue, 50);

	JBoolean found = te.ReplaceAll(
		JRegex("e"), kJTrue,
		JString("\xC3\xA9", 0, kJFalse), NULL, kJFalse, kJFalse);
	JAssertFalse(found);
	JAssertStringsEqual("Fourscore and seven years ago...", text.GetText());

	found = te.ReplaceAll(
		JRegex("e"), kJFalse,
		JString("\xC3\xA9", 0, kJFalse), NULL, kJFalse, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fourscor\xC3\xA9 and s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", text.GetText());
	JAssertEqual(charCount, text.GetText().GetCharacterCount());

	JRegex r1 = "four";
	r1.SetCaseSensitive(kJFalse);
	found = te.ReplaceAll(
		r1, kJTrue,
		JString("five", 0, kJFalse), NULL, kJTrue, kJFalse);
	JAssertFalse(found);	// caret at end

	found = te.ReplaceAll(
		r1, kJFalse,
		JString("five", 0, kJFalse), NULL, kJTrue, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 and s" "\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", text.GetText());

	JInterpolate interp;

	found = te.ReplaceAll(
		JRegex("a([^\\s]+)"), kJFalse,
		JString("fou$1", 0, kJFalse), &interp, kJFalse, kJFalse);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "fours fougo...", text.GetText());

	te.SetSelection(JCharacterRange(15, 22));
	found = te.ReplaceAll(
		JRegex("\xC3\xA9"), kJFalse,
		JString("e", 0, kJFalse), NULL, kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found seven y\xC3\xA9" "fours fougo...", text.GetText());

	te.SetSelection(JCharacterRange(text.GetText().GetCharacterCount() - 5, text.GetText().GetCharacterCount()));
	found = te.ReplaceAll(
		JRegex("o"), kJFalse,
		JString("\xC3\xB8", 0, kJFalse), NULL, kJFalse, kJTrue);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found seven y\xC3\xA9" "fours foug\xC3\xB8...", text.GetText());
}

class BigFontMatch : public JStyledText::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return JI2B( f.GetSize() == 20 );
	}
};

class BoldFontMatch : public JStyledText::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return f.GetStyle().bold;
	}
};

class ItalicFontMatch : public JStyledText::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return f.GetStyle().italic;
	}
};

class UnderlineFontMatch : public JStyledText::FontMatch
{
	virtual JBoolean Match(const JFont& f) const
	{
		return JI2B( f.GetStyle().underlineCount > 0 );
	}
};

JTEST(SearchStyle)
{
	StyledText text;

	TextEditor te(&text, kJFalse, 50);
	te.SetCurrentFontSize(20);
	te.Paste(JString("b" "\xC3\xAE" "g", 0, kJFalse));
	te.SetCurrentFontSize(JGetDefaultFontSize());
	te.SetCurrentFontBold(kJTrue);
	te.Paste(JString("b" "\xC3\xB8" "ld", 0, kJFalse));
	te.SetCurrentFontBold(kJFalse);
	te.Paste(JString("normal", 0, kJFalse));
	te.SetCurrentFontUnderline(2);
	te.Paste(JString("double underline", 0, kJFalse));
	te.SetCurrentFontUnderline(0);

	te.SetCaretLocation(1);

	// forward

	JBoolean wrapped;
	JBoolean found = te.SearchForward(ItalicFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertFalse(te.HasSelection());	// caret still at beginning
	JAssertFalse(wrapped);

	JString s;
	JCharacterRange r;
	found = te.SearchForward(BoldFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(4, 7), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xB8" "ld", s);
	JAssertFalse(wrapped);

	found = te.SearchForward(UnderlineFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("double underline", s);
	JAssertFalse(wrapped);

	found = te.SearchForward(BoldFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertTrue(te.GetSelection(&s));		// selection maintained
	JAssertStringsEqual("double underline", s);
	JAssertFalse(wrapped);

	found = te.SearchForward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertTrue(wrapped);

	// backward

	te.SetCaretLocation(20);

	te.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	found = te.SearchBackward(BoldFontMatch(), kJFalse, &wrapped);
	JAssertFalse(found);
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	found = te.SearchBackward(BoldFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(4, 7), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xB8" "ld", s);
	JAssertTrue(wrapped);

	te.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	te.SetCaretLocation(4);
	te.SearchBackward(BigFontMatch(), kJTrue, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);
}

JTEST(Selection)
{
	StyledText text;
	text.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", 0, kJFalse));

	TextEditor te(&text, kJTrue, 50);

	te.SetSelection(JCharacterRange(1, 5));
	te.Cut();
	JAssertStringsEqual("1234\nwxzy", text.GetText());

	te.SetCaretLocation(10);
	te.Paste(JString("\n", 0, kJFalse));
	te.Paste();
	JAssertStringsEqual("1234\nwxzy\n" "\xC3\xA1" "bcd\n", text.GetText());

	te.SetSelection(JCharacterRange(6, 10));
	te.DeleteSelection();
	JAssertStringsEqual("1234\n" "\xC3\xA1" "bcd\n", text.GetText());

	te.SelectAll();
	te.DeleteSelection();
	JAssertEqual(0, text.GetText().GetCharacterCount());
}

JTEST(TabSelection)
{
	StyledText text;
	text.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", 0, kJFalse));

	TextEditor te(&text, kJFalse, 50);

	te.TabSelectionRight(2);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	te.SetSelection(JCharacterRange(10, 13));
	te.TabSelectionLeft(3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	te.TabSelectionRight(3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", text.GetText());

	te.SelectAll();
	te.TabSelectionLeft(1);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", text.GetText());

	te.TabSelectionLeft(2);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", text.GetText());

	te.TabSelectionLeft(2, kJTrue);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());
}

JTEST(TabSelectionMixed)
{
	StyledText text;
	text.SetText(JString("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", 0, kJFalse));

	TextEditor te(&text, kJFalse, 50);

	te.SelectAll();
	te.TabSelectionLeft(1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	text.Undo();
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", text.GetText());

	text.Redo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());


	text.SetText(JString("  " "\xC3\xA1" "bcd\n   1234\n    wxzy", 0, kJFalse));

	te.SelectAll();
	te.TabSelectionLeft(1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n 1234\n  wxzy", text.GetText());
}

JTEST(SetAllFontNameAndSize)
{
	StyledText text;

	TextEditor te(&text, kJFalse, 50);
	te.SetCurrentFontSize(20);
	te.Paste(JString("b" "\xC3\xAE" "g", 0, kJFalse));
	te.SetCurrentFontSize(JGetDefaultFontSize());
	te.SetCurrentFontBold(kJTrue);
	te.Paste(JString("b" "\xC3\xB8" "ld", 0, kJFalse));
	te.SetCurrentFontBold(kJFalse);
	te.Paste(JString("normal", 0, kJFalse));
	te.SetCurrentFontUnderline(2);
	te.Paste(JString("double underline", 0, kJFalse));
	te.SetCurrentFontUnderline(0);

	te.SetAllFontNameAndSize(JString("foobar", 0, kJFalse), 15);

	JFont f = text.GetStyles().GetElement(2);
	JAssertStringsEqual("foobar", f.GetName());
	JAssertEqual(15, f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = text.GetStyles().GetElement(5);
	JAssertStringsEqual("foobar", f.GetName());
	JAssertEqual(15, f.GetSize());
	JAssertTrue(f.GetStyle().bold);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = text.GetStyles().GetElement(10);
	JAssertStringsEqual("foobar", f.GetName());
	JAssertEqual(15, f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertEqual(0, f.GetStyle().underlineCount);

	f = text.GetStyles().GetElement(17);
	JAssertStringsEqual("foobar", f.GetName());
	JAssertEqual(15, f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertEqual(2, f.GetStyle().underlineCount);
}
