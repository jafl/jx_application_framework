/******************************************************************************
 test_JTextEditor.cpp

	Program to test JTextEditor class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "TextEditor.h"
#include "StyledText.h"
#include "JVIKeyHandler.h"
#include "JRegex.h"
#include "JInterpolate.h"
#include "JFontManager.h"
#include "jFStreamUtil.h"
#include "jFileUtil.h"
#include "jASCIIConstants.h"
#include "jGlobals.h"
#include "jAssert.h"

using TextIndex = JStyledText::TextIndex;
using TextCount = JStyledText::TextCount;
using TextRange = JStyledText::TextRange;

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
/*113*/	"and dedicated to the proposition that all men are created equal.\n", JString::kNoCopy));

	TextEditor te(&text, true, 50);
/*
	std::cout << "=====" << std::endl;
	te.Draw();
	std::cout << "=====" << std::endl;
*/
	JAssertEqual(5, te.GetLineCount());
	JAssertEqual(65*7 + 12, te.GetWidth());
	JAssertEqual(6*7, te.GetHeight());
	JAssertEqual(1, te.GetLineForChar(8));
	JAssertEqual(2, te.GetLineForChar(50));
	JAssertEqual(21, te.GetLineTop(4));
	JAssertEqual(7, te.GetLineHeight(4));
	JAssertEqual(7, te.GetLineHeight(6));	// ends with newline
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
	JAssertEqual(76, charRange.last);

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
/*
	std::cout << "=====" << std::endl;
	te.Draw();
	std::cout << "=====" << std::endl;
*/
	JAssertEqual(30, te.GetLineCount());
	JAssertEqual(width, te.GetWidth());
	JAssertEqual(31*7, te.GetHeight());
	JAssertEqual(2, te.GetLineForChar(8));
	JAssertEqual(6, te.GetLineForChar(28));
	JAssertEqual(7, te.GetLineForChar(32));
	JAssertEqual(9, te.GetLineForChar(50));
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
	JAssertEqual(73, te.GetLineCharStart(13));
	JAssertEqual(77, te.GetLineCharStart(14));
	JAssertEqual(83, te.GetLineCharStart(15));
	JAssertEqual(91, te.GetLineCharStart(16));
	JAssertEqual(98, te.GetLineCharStart(17));
	JAssertEqual(104, te.GetLineCharStart(18));
	JAssertEqual(111, te.GetLineCharStart(19));
	JAssertEqual(113, te.GetLineCharStart(20));
	JAssertEqual(117, te.GetLineCharStart(21));
	JAssertEqual(124, te.GetLineCharStart(22));
	JAssertEqual(130, te.GetLineCharStart(23));
	JAssertEqual(134, te.GetLineCharStart(24));
	JAssertEqual(141, te.GetLineCharStart(25));
	JAssertEqual(146, te.GetLineCharStart(26));
	JAssertEqual(151, te.GetLineCharStart(27));
	JAssertEqual(159, te.GetLineCharStart(28));
	JAssertEqual(163, te.GetLineCharStart(29));
	JAssertEqual(171, te.GetLineCharStart(30));

	JAssertEqual(6, te.GetLineCharLength(2));
	JAssertEqual(11, te.GetLineCharEnd(2));

	JAssertEqual(8, te.GetLineCharLength(29));
	JAssertEqual(170, te.GetLineCharEnd(29));

	JAssertEqual(10, te.GetCharLeft(1));
	JAssertEqual(24, te.GetCharLeft(8));
	JAssertEqual(31, te.GetCharRight(8));
	JAssertEqual(31, te.GetCharLeft(9));

	JIndex charIndex;
	JCharacterRange charRange;

	te.GoToLine(100);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(text.GetText().GetCharacterCount()+1, charIndex);

	te.GoToLine(4);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(16, charIndex);

	te.GoToColumn(4, 10);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(22, charIndex);

	te.GoToColumn(4, 5);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(20, charIndex);

	te.GoToBeginningOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(16, charIndex);

	te.GoToEndOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(21, charIndex);

	te.SelectLine(2);
	JAssertFalse(te.GetCaretLocation(&charIndex));
	JAssertTrue(te.GetSelection(&charRange));
	JAssertEqual(6, charRange.first);
	JAssertEqual(11, charRange.last);

	te.GoToBeginningOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(6, charIndex);

	te.SelectLine(2);
	te.GoToEndOfLine();
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(11, charIndex);

	JAssertEqual(1, te.CalcCaretCharLocation(JPoint(0,0)));
	JAssertEqual(1, te.CalcCaretCharLocation(JPoint(13,6)));
	JAssertEqual(2, te.CalcCaretCharLocation(JPoint(14,6)));
	JAssertEqual(5, te.CalcCaretCharLocation(JPoint(62,3)));
	JAssertEqual(6, te.CalcCaretCharLocation(JPoint(13,7)));
	JAssertEqual(7, te.CalcCaretCharLocation(JPoint(14,9)));

	te.SelectLine(12);
	JAssertFalse(te.TestPointInSelection(JPoint(13,76)));
	JAssertTrue(te.TestPointInSelection(JPoint(13,77)));

	te.SetCaretLocation(8);
	JAssertFalse(te.TestPointInSelection(JPoint(13,76)));

	te.TestMoveCaretVert(+2);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(18, charIndex);

	te.TestMoveCaretVert(-1);
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(14, charIndex);
}

JTEST(LayoutBreakWidth)
{
	StyledText text;
	text.SetText(JString(
		"Four sc" "\xC3\xB8" "re and seven years ago\n"
		"our fathers brought forth on this continent,\n"
		"a new nation,\n"
		"conceived in Liberty,\n"
		"and dedicated to the proposition that all men are created equal.\n", JString::kNoCopy));

	TextEditor te(&text, false, 100);

	TestLayoutBreakWidth(text, te, 12 + 49);
	TestLayoutBreakWidth(text, te, 12 + 52);
	TestLayoutBreakWidth(text, te, 12 + 55);

	text.SetText(JString::empty);

	JAssertEqual(1, te.GetLineCount());
	JAssertEqual(7, te.GetHeight());
	JAssertEqual(0, te.GetLineTop(1));
	JAssertEqual(7, te.GetLineHeight(1));

	JAssertEqual(1, te.GetLineCharStart(1));
	JAssertEqual(1, te.GetLineCharEnd(1));

	JAssertEqual(1, te.CalcCaretCharLocation(JPoint(0,0)));

	text.SetText(JString("foo\n ", JString::kNoCopy));

	JAssertEqual(2, te.GetLineCount());
	JAssertEqual(14, te.GetHeight());
	JAssertEqual(7, te.GetLineTop(2));
	JAssertEqual(7, te.GetLineHeight(1));

	JAssertEqual(1, te.GetLineCharStart(1));
	JAssertEqual(4, te.GetLineCharLength(1));
	JAssertEqual(4, te.GetLineCharEnd(1));

	JAssertEqual(5, te.GetLineCharStart(2));
	JAssertEqual(1, te.GetLineCharLength(2));
	JAssertEqual(5, te.GetLineCharEnd(2));

	text.SetText(JString("foo", JString::kNoCopy));

	JAssertEqual(4, te.CalcCaretCharLocation(JPoint(100,0)));

	te.GoToColumn(1, 10);	// make sure it doesn't crash

	JIndex charIndex;
	JAssertTrue(te.GetCaretLocation(&charIndex));
	JAssertEqual(4, charIndex);
}

JTEST(GetCmdStatus)
{
	JArray<bool> status;
	for (JIndex i=1; i<=JTextEditor::kCmdCount; i++)
	{
		status.AppendElement(false);
	}

	StyledText text(true);
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", JString::kNoCopy));

	TextEditor te(&text, true, 50);
	te.Activate();

	status.SetElement(JTextEditor::kPasteCmd, true);
	status.SetElement(JTextEditor::kSelectAllCmd, true);
	status.SetElement(JTextEditor::kCheckSpellingCmd, true);
	status.SetElement(JTextEditor::kCleanRightMarginCmd, true);
	status.SetElement(JTextEditor::kCoerceRightMarginCmd, true);
	status.SetElement(JTextEditor::kShiftSelLeftCmd, true);
	status.SetElement(JTextEditor::kShiftSelRightCmd, true);
	status.SetElement(JTextEditor::kForceShiftSelLeftCmd, true);
	status.SetElement(JTextEditor::kShowWhitespaceCmd, true);
	status.SetElement(JTextEditor::kCleanAllWhitespaceCmd, true);
	status.SetElement(JTextEditor::kCleanAllWSAlignCmd, true);
	status.SetElement(JTextEditor::kToggleReadOnlyCmd, true);

	status.SetElement(JTextEditor::kFindDialogCmd, true);
	status.SetElement(JTextEditor::kFindClipboardBackwardCmd, true);
	status.SetElement(JTextEditor::kFindClipboardForwardCmd, true);

	std::cout << "GetCmdStatus::basic" << std::endl;
	te.CheckCmdStatus(status);

	te.SetHasSearchText(true);

	status.SetElement(JTextEditor::kFindPreviousCmd, true);
	status.SetElement(JTextEditor::kFindNextCmd, true);
	status.SetElement(JTextEditor::kReplaceAllCmd, true);

	std::cout << "GetCmdStatus::has search text" << std::endl;
	te.CheckCmdStatus(status);

	te.SetSelection(JCharacterRange(2,3));

	status.SetElement(JTextEditor::kCutCmd, true);
	status.SetElement(JTextEditor::kCopyCmd, true);
	status.SetElement(JTextEditor::kDeleteSelCmd, true);
	status.SetElement(JTextEditor::kCheckSpellingSelCmd, true);
	status.SetElement(JTextEditor::kCleanWhitespaceSelCmd, true);
	status.SetElement(JTextEditor::kCleanWSAlignSelCmd, true);
	status.SetElement(JTextEditor::kEnterSearchTextCmd, true);
	status.SetElement(JTextEditor::kEnterReplaceTextCmd, true);
	status.SetElement(JTextEditor::kFindSelectionBackwardCmd, true);
	status.SetElement(JTextEditor::kFindSelectionForwardCmd, true);
	status.SetElement(JTextEditor::kReplaceSelectionCmd, true);
	status.SetElement(JTextEditor::kReplaceFindPrevCmd, true);
	status.SetElement(JTextEditor::kReplaceFindNextCmd, true);
	status.SetElement(JTextEditor::kReplaceAllInSelectionCmd, true);

	std::cout << "GetCmdStatus::has selection" << std::endl;
	te.CheckCmdStatus(status);

	te.DeleteSelection();

	status.SetElement(JTextEditor::kUndoCmd, true);

	status.SetElement(JTextEditor::kCutCmd, false);
	status.SetElement(JTextEditor::kCopyCmd, false);
	status.SetElement(JTextEditor::kDeleteSelCmd, false);
	status.SetElement(JTextEditor::kCheckSpellingSelCmd, false);
	status.SetElement(JTextEditor::kCleanWhitespaceSelCmd, false);
	status.SetElement(JTextEditor::kCleanWSAlignSelCmd, false);
	status.SetElement(JTextEditor::kEnterSearchTextCmd, false);
	status.SetElement(JTextEditor::kEnterReplaceTextCmd, false);
	status.SetElement(JTextEditor::kFindSelectionBackwardCmd, false);
	status.SetElement(JTextEditor::kFindSelectionForwardCmd, false);
	status.SetElement(JTextEditor::kReplaceSelectionCmd, false);
	status.SetElement(JTextEditor::kReplaceFindPrevCmd, false);
	status.SetElement(JTextEditor::kReplaceFindNextCmd, false);
	status.SetElement(JTextEditor::kReplaceAllInSelectionCmd, false);

	std::cout << "GetCmdStatus::has undo" << std::endl;
	te.CheckCmdStatus(status);

	text.Undo();

	status.SetElement(JTextEditor::kUndoCmd, false);
	status.SetElement(JTextEditor::kRedoCmd, true);

	status.SetElement(JTextEditor::kCutCmd, true);
	status.SetElement(JTextEditor::kCopyCmd, true);
	status.SetElement(JTextEditor::kDeleteSelCmd, true);
	status.SetElement(JTextEditor::kCheckSpellingSelCmd, true);
	status.SetElement(JTextEditor::kCleanWhitespaceSelCmd, true);
	status.SetElement(JTextEditor::kCleanWSAlignSelCmd, true);
	status.SetElement(JTextEditor::kEnterSearchTextCmd, true);
	status.SetElement(JTextEditor::kEnterReplaceTextCmd, true);
	status.SetElement(JTextEditor::kFindSelectionBackwardCmd, true);
	status.SetElement(JTextEditor::kFindSelectionForwardCmd, true);
	status.SetElement(JTextEditor::kReplaceSelectionCmd, true);
	status.SetElement(JTextEditor::kReplaceFindPrevCmd, true);
	status.SetElement(JTextEditor::kReplaceFindNextCmd, true);
	status.SetElement(JTextEditor::kReplaceAllInSelectionCmd, true);

	std::cout << "GetCmdStatus::has redo" << std::endl;
	te.CheckCmdStatus(status);
}

JTEST(SetText)
{
	StyledText text;
	text.SetText(JString("abcdefgh", JString::kNoCopy));

	TextEditor te(&text, true, 100);

	te.SetCaretLocation(100);

	// triggers TextEditor::Receive()
	text.SetText(JString("abcd", JString::kNoCopy));
}

JTEST(SearchTextForward)
{
	StyledText text;
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", JString::kNoCopy));

	TextEditor te(&text, true, 50);

	// entire word, no wrapping

	bool wrapped;
	JIndex caretIndex;
	const JStringMatch m1 = te.SearchForward(JRegex("sc" "\xC3\xB8" "re"), true, false, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(te.HasSelection());	// caret still at beginning
	JAssertTrue(te.GetCaretLocation(&caretIndex));
	JAssertEqual(1, caretIndex);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	JString s;
	JCharacterRange r;
	const JStringMatch m2 = te.SearchForward(JRegex("sc" "\xC3\xB8" "re"), false, false, &wrapped);
	JAssertStringsEqual("sc" "\xC3\xB8" "re", m2.GetString());
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(5, 9), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("sc" "\xC3\xB8" "re", s);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m3 = te.SearchForward(JRegex("\xC3\xB8" "re and"), false, false, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(5, 9), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("sc" "\xC3\xB8" "re", s);
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = te.SearchForward(JRegex("\xC3\xB8" "re and"), false, true, &wrapped);
	JAssertStringsEqual("\xC3\xB8" "re and", m4.GetString());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(7, 13), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("\xC3\xB8" "re and", s);
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = te.SearchForward(JRegex("year"), true, false, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(7, 13), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("\xC3\xB8" "re and", s);
	JAssertFalse(wrapped);

	const JStringMatch m7 = te.SearchForward(JRegex("years"), true, false, &wrapped);
	JAssertStringsEqual("years", m7.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// last characters

	te.SetCaretLocation(te.GetText()->GetText().GetCharacterCount() - 2);
	JAssertFalse(te.HasSelection());
	const JStringMatch m5 = te.SearchForward(JRegex("\\.\\.\\."), false, true, &wrapped);
	JAssertStringsEqual("...", m5.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("...", s);
	JAssertFalse(wrapped);

	const JStringMatch m8 = te.SelectionMatches(JRegex("\\.\\.\\."), false);
	JAssertFalse(m8.IsEmpty());
	JAssertStringsEqual("...", m8.GetString());
}

JTEST(SearchTextBackward)
{
	StyledText text;
	text.SetText(JString("Fourscore and seve" "\xC3\xB1" " years ago...", JString::kNoCopy));

	TextEditor te(&text, false, 50);
	te.SetCaretLocation(text.GetText().GetCharacterCount() + 1);

	// entire word, no wrapping

	bool wrapped;
	const JStringMatch m1 = te.SearchBackward(JRegex("year"), true, false, &wrapped);
	JAssertTrue(m1.IsEmpty());
	JAssertFalse(te.HasSelection());	// caret still at end
	JAssertFalse(wrapped);

	JString s;
	JCharacterRange r;
	const JStringMatch m2 = te.SearchBackward(JRegex("years"), true, false, &wrapped);
	JAssertStringsEqual("years", m2.GetString());
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(21, 25), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// partial word, no wrapping

	const JStringMatch m3 = te.SearchBackward(JRegex("e" "\xC3\xB1" " ye"), false, false, &wrapped);
	JAssertTrue(m3.IsEmpty());
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(21, 25), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("years", s);
	JAssertFalse(wrapped);

	// partial word, wrapping

	const JStringMatch m4 = te.SearchBackward(JRegex("e" "\xC3\xB1" " ye"), false, true, &wrapped);
	JAssertStringsEqual("e" "\xC3\xB1" " ye", m4.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("e" "\xC3\xB1" " ye", s);
	JAssertTrue(wrapped);

	// entire word, no wrapping

	const JStringMatch m6 = te.SearchBackward(JRegex("an"), true, false, &wrapped);
	JAssertTrue(m6.IsEmpty());
	JAssertTrue(te.GetSelection(&s));		// selection maintained
	JAssertStringsEqual("e" "\xC3\xB1" " ye", s);
	JAssertFalse(wrapped);

	const JStringMatch m7 = te.SearchBackward(JRegex("and"), true, false, &wrapped);
	JAssertStringsEqual("and", m7.GetString());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("and", s);
	JAssertFalse(wrapped);

	// first characters

	te.SetCaretLocation(5);
	JAssertFalse(te.HasSelection());
	const JStringMatch m5 = te.SearchBackward(JRegex("Four"), false, true, &wrapped);
	JAssertStringsEqual("Four", m5.GetString());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 4), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("Four", s);
	JAssertFalse(wrapped);
}

JTEST(ReplaceSelection)
{
	StyledText text;
	text.SetText(JString("Foursc" "\xC3\xB8" "re and seven years ago...", JString::kNoCopy));

	TextEditor te(&text, false, 50);

	bool wrapped;
	const JStringMatch m1 = te.SearchForward(JRegex("sc" "\xC3\xB8" "re"), false, false, &wrapped);
	JAssertTrue(te.HasSelection());

	JString s;
	te.TestReplaceSelection(m1, JString("s" "\xC3\xA7" "or" "\xC3\xA9", JString::kNoCopy), nullptr, false);
	JAssertStringsEqual("Fours" "\xC3\xA7" "or" "\xC3\xA9" " and seven years ago...", te.GetText()->GetText());
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("s" "\xC3\xA7" "or" "\xC3\xA9", s);

	const JStringMatch m2 = te.SearchForward(JRegex("and"), false, false, &wrapped);
	JAssertTrue(te.HasSelection());

	JIndex i;
	te.TestReplaceSelection(m2, JString::empty, nullptr, false);
	JAssertStringsEqual("Fours" "\xC3\xA7" "or" "\xC3\xA9" "  seven years ago...", te.GetText()->GetText());
	JAssertFalse(te.HasSelection());
	JAssertTrue(te.GetCaretLocation(&i));
	JAssertEqual(11, i);
}

JTEST(ReplaceAll)
{
	StyledText text;
	text.SetText(JString("Fourscore and seven years ago...", JString::kNoCopy));
	const JSize charCount = text.GetText().GetCharacterCount();

	TextEditor te(&text, true, 50);

	bool found = te.ReplaceAll(
		JRegex("e"), true,
		JString("\xC3\xA9", JString::kNoCopy), nullptr, false, false);
	JAssertFalse(found);
	JAssertStringsEqual("Fourscore and seven years ago...", text.GetText());

	found = te.ReplaceAll(
		JRegex("e"), false,
		JString("\xC3\xA9", JString::kNoCopy), nullptr, false, false);
	JAssertTrue(found);
	JAssertStringsEqual("Fourscor\xC3\xA9 and s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", text.GetText());
	JAssertEqual(charCount, text.GetText().GetCharacterCount());

	JRegex r1 = "four";
	r1.SetCaseSensitive(false);
	found = te.ReplaceAll(
		r1, true,
		JString("five", false), nullptr, true, false);
	JAssertFalse(found);	// caret at end

	found = te.ReplaceAll(
		r1, false,
		JString("five", JString::kNoCopy), nullptr, true, false);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 and s" "\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "ars ago...", text.GetText());

	JInterpolate interp;

	found = te.ReplaceAll(
		JRegex("a([^\\s]+)"), false,
		JString("fou$1", JString::kNoCopy), &interp, false, false);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found s\xC3\xA9" "v\xC3\xA9" "n y\xC3\xA9" "fours fougo...", text.GetText());

	te.SetSelection(JCharacterRange(15, 22));
	found = te.ReplaceAll(
		JRegex("\xC3\xA9"), false,
		JString("e", JString::kNoCopy), nullptr, false, true);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found seven y\xC3\xA9" "fours fougo...", text.GetText());

	te.SetSelection(JCharacterRange(text.GetText().GetCharacterCount() - 5, text.GetText().GetCharacterCount()));
	found = te.ReplaceAll(
		JRegex("o"), false,
		JString("\xC3\xB8", JString::kNoCopy), nullptr, false, true);
	JAssertTrue(found);
	JAssertStringsEqual("Fivescor\xC3\xA9 found seven y\xC3\xA9" "fours foug\xC3\xB8...", text.GetText());
}

bool bigFontMatch(const JFont& f)
{
	return f.GetSize() == 20;
}

bool boldFontMatch(const JFont& f)
{
	return f.GetStyle().bold;
}

JTEST(SearchStyle)
{
	StyledText text;

	TextEditor te(&text, false, 50);
	te.SetCurrentFontSize(20);
	te.Paste(JString("b" "\xC3\xAE" "g", JString::kNoCopy));
	te.SetCurrentFontSize(JFontManager::GetDefaultFontSize());
	te.SetCurrentFontBold(true);
	te.Paste(JString("b" "\xC3\xB8" "ld", JString::kNoCopy));
	te.SetCurrentFontBold(false);
	te.Paste(JString("normal", false));
	te.SetCurrentFontUnderline(2);
	te.Paste(JString("double underline", JString::kNoCopy));
	te.SetCurrentFontUnderline(0);

	te.SetCaretLocation(1);

	// forward

	bool wrapped;
	bool found = te.SearchForward([] (const JFont& f)
	{
		return f.GetStyle().italic;
	},
		false, &wrapped);

	JAssertFalse(found);
	JAssertFalse(te.HasSelection());	// caret still at beginning
	JAssertFalse(wrapped);

	JString s;
	JCharacterRange r;
	found = te.SearchForward(boldFontMatch, true, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.HasSelection());
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(4, 7), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xB8" "ld", s);
	JAssertFalse(wrapped);

	found = te.SearchForward([] (const JFont& f)
	{
		return f.GetStyle().underlineCount > 0;
	},
		true, &wrapped);

	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("double underline", s);
	JAssertFalse(wrapped);

	found = te.SearchForward(boldFontMatch, false, &wrapped);
	JAssertFalse(found);
	JAssertTrue(te.GetSelection(&s));		// selection maintained
	JAssertStringsEqual("double underline", s);
	JAssertFalse(wrapped);

	found = te.SearchForward(bigFontMatch, true, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertTrue(wrapped);

	// backward

	te.SetCaretLocation(20);

	te.SearchBackward(bigFontMatch, true, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	found = te.SearchBackward(boldFontMatch, false, &wrapped);
	JAssertFalse(found);
	JAssertTrue(te.GetSelection(&r));		// selection maintained
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	found = te.SearchBackward(boldFontMatch, true, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(4, 7), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xB8" "ld", s);
	JAssertTrue(wrapped);

	te.SearchBackward(bigFontMatch, true, &wrapped);
	JAssertTrue(found);
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(JCharacterRange(1, 3), r);
	JAssertTrue(te.GetSelection(&s));
	JAssertStringsEqual("b" "\xC3\xAE" "g", s);
	JAssertFalse(wrapped);

	te.SetCaretLocation(4);
	te.SearchBackward(bigFontMatch, true, &wrapped);
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
	text.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", JString::kNoCopy));

	TextEditor te(&text, true, 50);

	te.SetSelection(JCharacterRange(1, 5));
	te.Cut();
	JAssertStringsEqual("1234\nwxzy", text.GetText());

	te.SetCaretLocation(10);
	te.Paste(JString::newline);
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
	text.SetText(JString("\xC3\xA1" "bcd\n1234\nwxzy", JString::kNoCopy));

	TextEditor te(&text, false, 50);

	te.TabSelectionRight(2);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	te.SetSelection(JCharacterRange(10, 13));
	te.TabSelectionLeft(3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	te.TabSelectionRight(3);
	JAssertStringsEqual("\t\t" "\xC3\xA1" "bcd\n\t\t\t1234\n\t\t\twxzy", text.GetText());

	JCharacterRange r;
	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(8, r.first);
	JAssertEqual(22, r.last);

	te.SelectAll();
	te.TabSelectionLeft(1);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", text.GetText());

	te.TabSelectionLeft(2);
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n\t\t1234\n\t\twxzy", text.GetText());

	te.TabSelectionLeft(2, true);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	// edge cases

	text.SetText(JString("abc\n\n\t\txyz", JString::kNoCopy));

	te.SetCaretLocation(5);
	te.TabSelectionLeft();
	JAssertStringsEqual("abc\n\n\t\txyz", text.GetText());
	JAssertFalse(te.GetSelection(&r));

	te.TabSelectionRight();
	JAssertStringsEqual("abc\n\n\t\txyz", text.GetText());
	JAssertFalse(te.GetSelection(&r));

	te.SetCaretLocation(6);
	te.TabSelectionLeft();
	JAssertStringsEqual("abc\n\n\txyz", text.GetText());

	JAssertTrue(te.GetSelection(&r));
	JAssertEqual(6, r.first);
	JAssertEqual(9, r.last);
}

JTEST(TabSelectionMixed)
{
	StyledText text;
	text.SetText(JString("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", JString::kNoCopy));

	TextEditor te(&text, false, 50);

	te.SelectAll();
	te.TabSelectionLeft(1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());

	text.Undo();
	JAssertStringsEqual("\t" "\xC3\xA1" "bcd\n  \t1234\n\twxzy", text.GetText());

	text.Redo();
	JAssertStringsEqual("\xC3\xA1" "bcd\n1234\nwxzy", text.GetText());


	text.SetText(JString("  " "\xC3\xA1" "bcd\n   1234\n    wxzy", JString::kNoCopy));

	te.SelectAll();
	te.TabSelectionLeft(1);
	JAssertStringsEqual("\xC3\xA1" "bcd\n 1234\n  wxzy", text.GetText());
}

JTEST(SetAllFontNameAndSize)
{
	StyledText text(true);

	TextEditor te(&text, false, 50);
	te.SetCurrentFontSize(20);
	te.Paste(JString("replace this", JString::kNoCopy));
	te.SelectAll();
	te.Paste(JString("b" "\xC3\xAE" "g", JString::kNoCopy));
	te.SetCurrentFontSize(JFontManager::GetDefaultFontSize());
	te.SetCurrentFontBold(true);
	te.Paste(JString("b" "\xC3\xB8" "ld", JString::kNoCopy));
	te.SetCurrentFontBold(false);
	te.Paste(JString("normal", JString::kNoCopy));
	te.SetCurrentFontUnderline(2);
	te.Paste(JString("double underline", JString::kNoCopy));
	te.SetCurrentFontUnderline(0);

	te.SetAllFontNameAndSize(JString("foobar", JString::kNoCopy), 15);

	JRunArrayIterator<JFont> iter(text.GetStyles());
	JFont f;

	iter.MoveTo(JListT::kStartBefore, 2);
	iter.Next(&f);
	JAssertStringsEqual("foobar", f.GetName());
	JAssertEqual(15, f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertEqual(0, f.GetStyle().underlineCount);

	iter.MoveTo(JListT::kStartBefore, 5);
	iter.Next(&f);
	JAssertStringsEqual("foobar", f.GetName());
	JAssertEqual(15, f.GetSize());
	JAssertTrue(f.GetStyle().bold);
	JAssertEqual(0, f.GetStyle().underlineCount);

	iter.MoveTo(JListT::kStartBefore, 10);
	iter.Next(&f);
	JAssertStringsEqual("foobar", f.GetName());
	JAssertEqual(15, f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertEqual(0, f.GetStyle().underlineCount);

	iter.MoveTo(JListT::kStartBefore, 17);
	iter.Next(&f);
	JAssertStringsEqual("foobar", f.GetName());
	JAssertEqual(15, f.GetSize());
	JAssertFalse(f.GetStyle().bold);
	JAssertEqual(2, f.GetStyle().underlineCount);
}

JTEST(GetDoubleClickSelection)
{
	StyledText text;
	text.SetText(JString("b fooBar  flip23bar k", JString::kNoCopy));

	TextEditor te(&text, true, 50);

	// foo

	TextRange r;
	te.GetDoubleClickSelection(TextIndex(4,4), false, false, &r);
	JAssertEqual(JCharacterRange(3,8), r.charRange);

	te.GetDoubleClickSelection(TextIndex(4,4), true, false, &r);
	JAssertEqual(JCharacterRange(3,5), r.charRange);

	// Bar

	te.GetDoubleClickSelection(TextIndex(8,8), false, false, &r);
	JAssertEqual(JCharacterRange(3,8), r.charRange);

	te.GetDoubleClickSelection(TextIndex(8,8), true, false, &r);
	JAssertEqual(JCharacterRange(6,8), r.charRange);

	// space

	te.GetDoubleClickSelection(TextIndex(10,10), false, false, &r);
	JAssertEqual(JCharacterRange(10,10), r.charRange);

	te.GetDoubleClickSelection(TextIndex(10,10), true, false, &r);
	JAssertEqual(JCharacterRange(10,10), r.charRange);

	// flip

	te.GetDoubleClickSelection(TextIndex(12,12), false, false, &r);
	JAssertEqual(JCharacterRange(11,19), r.charRange);

	te.GetDoubleClickSelection(TextIndex(12,12), true, false, &r);
	JAssertEqual(JCharacterRange(11,14), r.charRange);

	// 23

	te.GetDoubleClickSelection(TextIndex(16,16), false, false, &r);
	JAssertEqual(JCharacterRange(11,19), r.charRange);

	te.GetDoubleClickSelection(TextIndex(16,16), true, false, &r);
	JAssertEqual(JCharacterRange(15,16), r.charRange);

	// bar

	te.GetDoubleClickSelection(TextIndex(17,17), false, false, &r);
	JAssertEqual(JCharacterRange(11,19), r.charRange);

	te.GetDoubleClickSelection(TextIndex(17,17), true, false, &r);
	JAssertEqual(JCharacterRange(17,19), r.charRange);
}

JTEST(DefaultKeyHandler)
{
	StyledText text;
	TextEditor te1(&text, true, 50);
	TextEditor te2(&text, true, 50);

	text.TabShouldInsertSpaces(true);
	text.SetCRMTabCharCount(4);

	te1.Activate();
	te1.HandleKeyPress(JUtf8Character('c'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('t'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cat", text.GetText());

	te2.Activate();
	te2.HandleKeyPress(JUtf8Character('s'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cats", text.GetText());

	te2.SetCaretLocation(1);
	te2.HandleKeyPress(JUtf8Character('z'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("zcats", text.GetText());

	te1.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("zcatsa", text.GetText());

	te2.HandleKeyPress(JUtf8Character(kJDeleteKey), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("catsa", text.GetText());

	te1.HandleKeyPress(JUtf8Character(kJDeleteKey), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cats", text.GetText());

	te1.Paste(JString("Dog", JString::kNoCopy));
	te1.HandleKeyPress(JUtf8Character('s'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("catsDogs", text.GetText());

	te2.GoToEndOfLine();
	text.Undo();
	JAssertStringsEqual("catsDog", text.GetText());

	te2.HandleKeyPress(JUtf8Character(kJLeftArrow), false, JTextEditor::kMoveByWord, false);
	JIndex i;
	JAssertTrue(te2.GetCaretLocation(&i));
	JAssertEqual(1, i);

	te2.HandleKeyPress(JUtf8Character(kJRightArrow), false, JTextEditor::kMoveByPartialWord, false);
	JAssertTrue(te2.GetCaretLocation(&i));
	JAssertEqual(5, i);

	te1.HandleKeyPress(JUtf8Character(kJLeftArrow), true, JTextEditor::kMoveByPartialWord, false);
	JCharacterRange r;
	JAssertTrue(te1.GetSelection(&r));
	JAssertEqual(JCharacterRange(5,7), r);

	te1.HandleKeyPress(JUtf8Character('!'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cats!", text.GetText());

	te2.HandleKeyPress(JUtf8Character(kJTabKey), false, JTextEditor::kMoveByCharacter, false);
	te2.HandleKeyPress(JUtf8Character(' '), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cats     !", text.GetText());

	te2.HandleKeyPress(JUtf8Character(kJDeleteKey), false, JTextEditor::kMoveByCharacter, true);
	JAssertStringsEqual("cats    !", text.GetText());

	te2.HandleKeyPress(JUtf8Character(kJDeleteKey), false, JTextEditor::kMoveByCharacter, true);
	JAssertStringsEqual("cats!", text.GetText());

	te1.HandleKeyPress(JUtf8Character(kJLeftArrow), true, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJForwardDeleteKey), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cats", text.GetText());
}

JTEST(VIKeyHandler)
{
	StyledText text(true);
	TextEditor te1(&text, true, 50);
	TextEditor te2(&text, true, 50);

	text.TabShouldInsertSpaces(true);
	text.SetCRMTabCharCount(4);

	te1.SetKeyHandler(jnew JVIKeyHandler);
	te2.SetKeyHandler(jnew JVIKeyHandler);

	te1.Activate();
	te1.HandleKeyPress(JUtf8Character('i'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('c'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('t'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cat", text.GetText());

	te2.Activate();
	te2.HandleKeyPress(JUtf8Character('i'), false, JTextEditor::kMoveByCharacter, false);
	te2.HandleKeyPress(JUtf8Character('s'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cats", text.GetText());

	te2.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te2.HandleKeyPress(JUtf8Character('0'), false, JTextEditor::kMoveByCharacter, false);
	te2.HandleKeyPress(JUtf8Character('i'), false, JTextEditor::kMoveByCharacter, false);
	te2.HandleKeyPress(JUtf8Character('z'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("zcats", text.GetText());

	te1.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("zcatsa", text.GetText());

	te2.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te2.HandleKeyPress(JUtf8Character('x'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("catsa", text.GetText());

	te1.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('x'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("cats", text.GetText());

	te1.HandleKeyPress(JUtf8Character('I'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('z'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("zcats", text.GetText());

	te1.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJRightArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJLeftArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('A'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("zcaatsa", text.GetText());

	te1.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('O'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('1'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJDownArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('o'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('2'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nzcaatsa\n2", text.GetText());

	te1.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);

	JIndex i;
	JAssertTrue(te1.GetCaretLocation(&i));
	JAssertEqual(1, i);

	te1.HandleKeyPress(JUtf8Character('G'), false, JTextEditor::kMoveByCharacter, false);

	JAssertTrue(te1.GetCaretLocation(&i));
	JAssertEqual(12, i);

	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJRightArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJRightArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJRightArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('x'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('X'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nzcasa\n2", text.GetText());

	te1.HandleKeyPress(JUtf8Character('G'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('P'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nzcasa\n2a", text.GetText());

	te1.Paste(JString("\n3\n4\n5\n6\n7\n8\n9", JString::kNoCopy));
	te1.HandleKeyPress(JUtf8Character(kJEscapeKey), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('3'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('d'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('d'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nzcasa\n5\n6\n7\n8\n9", text.GetText());

	te1.HandleKeyPress(JUtf8Character('G'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('p'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nzcasa\n5\n6\n7\n8\n2a\n3\n4\n9", text.GetText());

	te2.HandleKeyPress(JUtf8Character('u'), false, JTextEditor::kMoveByCharacter, false);
	te2.HandleKeyPress(JUtf8Character('u'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nzcasa\n2a\n3\n4\n5\n6\n7\n8\n9", text.GetText());

	te1.HandleKeyPress(JUtf8Character(kJUpArrow), false, JTextEditor::kMoveByLine, false);
	te1.HandleKeyPress(JUtf8Character(kJDownArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJRightArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('D'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nz\n2a\n3\n4\n5\n6\n7\n8\n9", text.GetText());

	te2.HandleKeyPress(JUtf8Character('u'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJDownArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character(kJDownArrow), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('3'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('"'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('q'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('d'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('d'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nzcasa\n5\n6\n7\n8\n9", text.GetText());

	te1.HandleKeyPress(JUtf8Character('G'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('"'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('q'), false, JTextEditor::kMoveByCharacter, false);
	te1.HandleKeyPress(JUtf8Character('p'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("1\nzcasa\n5\n6\n7\n8\n2a\n3\n4\n9", text.GetText());
}

JTEST(AutoIndent)
{
	StyledText text;
	text.ShouldAutoIndent();
	text.SetCRMTabCharCount(4);

	TextEditor te(&text, true, 50);

	te.Activate();
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\t'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("\n\n\n\t", text.GetText());

	te.HandleKeyPress(JUtf8Character('\t'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("\n\n\n\t\ta\n\t\t", text.GetText());

	te.HandleKeyPress(JUtf8Character('\b'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\b'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character(' '), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character(' '), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\t'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character(' '), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('a'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("\n\n\n\t\ta\n  \t a\n  \t ", text.GetText());

	text.SetText(JString::empty);

	JStyledText::CRMRuleList rules;
	rules.AppendElement(JStyledText::CRMRule(
		JString("([[:space:]]*)/(\\*+[[:space:]]*)+", JString::kNoCopy),
		JString("[[:space:]]*(\\*+/?[[:space:]]*)+", JString::kNoCopy),
		JString("$1 * ", JString::kNoCopy)));
	rules.AppendElement(JStyledText::CRMRule(
		JString("[[:space:]]*(\\*+[[:space:]]*)+", JString::kNoCopy),
		JString("[[:space:]]*(\\*+/?[[:space:]]*)+", JString::kNoCopy),
		JString("$0", JString::kNoCopy)));
	rules.AppendElement(JStyledText::CRMRule(
		JString("[[:space:]]*//[[:space:]]*", JString::kNoCopy),
		JString("[[:space:]]*//[[:space:]]*", JString::kNoCopy),
		JString("$0", JString::kNoCopy)));

	text.SetCRMRuleList(&rules, false);

	te.HandleKeyPress(JUtf8Character('/'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('*'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("/*\n * ", text.GetText());
	te.HandleKeyPress(JUtf8Character('\b'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('*'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character(' '), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('x'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("/*\n ** x\n ** ", text.GetText());
	te.HandleKeyPress(JUtf8Character('\b'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('/'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("/*\n ** x\n **/\n ", text.GetText());

	te.HandleKeyPress(JUtf8Character('\t'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('/'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('/'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("/*\n ** x\n **/\n \t//\n \t//", text.GetText());
	te.HandleKeyPress(JUtf8Character(' '), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('x'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('y'), false, JTextEditor::kMoveByCharacter, false);
	te.HandleKeyPress(JUtf8Character('\n'), false, JTextEditor::kMoveByCharacter, false);
	JAssertStringsEqual("/*\n ** x\n **/\n \t//\n \t// xy\n \t// ", text.GetText());

	rules.DeleteAll();
}
