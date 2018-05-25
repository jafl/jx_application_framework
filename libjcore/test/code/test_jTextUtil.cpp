/******************************************************************************
 test_jTextUtil.cc

	Program to test jTextUtil.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <jTextUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(AnalyzeWhitespaceDetection)
{
	JString buffer;
	JBoolean useSpaces, isMixed;

	JSize tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 2, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "   abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	buffer   = "\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "   abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "   abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "   abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 2, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(3, tabWidth);

	buffer   = "   abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 2, kJFalse, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(2, tabWidth);

	buffer   = "    abcd\n\tabcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "abc\n foobar\n\n\txyz\n\nxyz\n";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);
}

JTEST(AnalyzeWhitespaceWidth)
{
	JString buffer;
	JBoolean useSpaces, isMixed;
	JSize tabWidth;

	buffer   = "   abcd\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	buffer   = "   abcd\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	buffer   = "abcd\n \nabcd\n\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "abcd\n \nabcd\n\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "abcd\n  \nabcd\n\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	buffer   = "abcd\n  \nabcd\n\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 2,4
	buffer   = "  abcd\n  \nabcd\n\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 2,3,4
	buffer   = "  abcd\n  \nabcd\n   abcd\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 2,4,5
	buffer   = "  abcd\n  \nabcd\n     abcd\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 3,4
	buffer   = "   abcd\n   \nabcd\n\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,6
	buffer   = "   abcd\n   \nabcd\n\n      abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,6
	buffer   = "   abcd\n   \nabcd\n      abcd\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,6,7
	buffer   = "   abcd\n   \nabcd\n      abcd\n    abcd\n       abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,6,8 - 3
	buffer   = "   abcd\n   \nabcd\n      abcd\n    abcd\n        abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,6,8 - 2
	buffer   = "   abcd\n    \nabcd\n      abcd\n    abcd\n        abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 3,4,8,9 - 3
	buffer   = "   abcd\n   \nabcd\n         abcd\n    abcd\n        abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,8,9 - 4
	buffer   = "   abcd\n    \nabcd\n         abcd\n    abcd\n        abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);
}

JTEST(Markdown)
{
	JStyledText st(kJFalse, kJTrue);

	JReadLimitedMarkdown(JString("hello", 0, kJFalse), &st);
	JAssertStringsEqual("hello", st.GetText());

	JReadLimitedMarkdown(JString("hello *bold* _italic_ `tt` bye!", 0, kJFalse), &st);
	JAssertStringsEqual("hello bold italic tt bye!", st.GetText());
	JAssertTrue(st.GetStyles().GetElement(8).GetStyle().bold);
	JAssertFalse(st.GetStyles().GetElement(8).GetStyle().italic);
	JAssertFalse(st.GetStyles().GetElement(15).GetStyle().bold);
	JAssertTrue(st.GetStyles().GetElement(15).GetStyle().italic);
	JAssertFalse(st.GetStyles().GetElement(19).GetStyle().bold);
	JAssertFalse(st.GetStyles().GetElement(19).GetStyle().italic);
	JAssertStringsEqual(JGetDefaultMonospaceFontName(), st.GetStyles().GetElement(19).GetName());
	JAssertFalse(st.GetStyles().GetElement(22).GetStyle().bold);
	JAssertFalse(st.GetStyles().GetElement(22).GetStyle().italic);
	JAssertStringsEqual(JGetDefaultFontName(), st.GetStyles().GetElement(22).GetName());
/*
	JReadLimitedMarkdown(JString("_w*b*w_ _*b`t`b*_", 0, kJFalse), &st);
	JAssertStringsEqual("wbw btb", st.GetText());
	JAssertFalse(st.GetStyles().GetElement(1).GetStyle().bold);
	JAssertTrue(st.GetStyles().GetElement(1).GetStyle().italic);
	JAssertTrue(st.GetStyles().GetElement(2).GetStyle().bold);
	JAssertTrue(st.GetStyles().GetElement(2).GetStyle().italic);
	JAssertFalse(st.GetStyles().GetElement(3).GetStyle().bold);
	JAssertTrue(st.GetStyles().GetElement(3).GetStyle().italic);
	JAssertFalse(st.GetStyles().GetElement(4).GetStyle().bold);
	JAssertFalse(st.GetStyles().GetElement(4).GetStyle().italic);
	JAssertTrue(st.GetStyles().GetElement(5).GetStyle().bold);
	JAssertTrue(st.GetStyles().GetElement(5).GetStyle().italic);
	JAssertStringsEqual(JGetDefaultFontName(), st.GetStyles().GetElement(5).GetName());
	JAssertTrue(st.GetStyles().GetElement(6).GetStyle().bold);
	JAssertTrue(st.GetStyles().GetElement(6).GetStyle().italic);
	JAssertStringsEqual(JGetDefaultMonospaceFontName(), st.GetStyles().GetElement(6).GetName());
	JAssertTrue(st.GetStyles().GetElement(7).GetStyle().bold);
	JAssertTrue(st.GetStyles().GetElement(7).GetStyle().italic);
	JAssertStringsEqual(JGetDefaultFontName(), st.GetStyles().GetElement(7).GetName());
*/
}
