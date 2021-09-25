/******************************************************************************
 test_jTextUtil.cpp

	Program to test jTextUtil.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <jTextUtil.h>
#include <JStyledText.h>
#include <JFontManager.h>
#include <JColorManager.h>
#include <jFStreamUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(PasteUNIXTerminalOutput)
{
	JStyledText text(true, true);

	JPasteUNIXTerminalOutput(JString(
		"n" "\033[1m" "b" "\033[4m" "bu" "\033[22m" "u" "\033[24m"
		"\033[3m" "i" "\033[31m" "ir" "\033[23m" "r" "\033[30m" "n",
		JString::kNoCopy), JStyledText::TextIndex(1,1), &text);
	JAssertStringsEqual("n" "b" "bu" "u" "i" "ir" "r" "n", text.GetText());

	JFont f = text.GetFont(1);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);

	f = text.GetFont(2);
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);

	f = text.GetFont(3);
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(1, f.GetStyle().underlineCount);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);

	f = text.GetFont(5);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(1, f.GetStyle().underlineCount);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);

	f = text.GetFont(6);
	JAssertFalse(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);

	f = text.GetFont(8);
	JAssertFalse(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertEqual(JColorManager::GetDarkRedColor(), f.GetStyle().color);

	f = text.GetFont(9);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertEqual(JColorManager::GetDarkRedColor(), f.GetStyle().color);

	f = text.GetFont(10);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertEqual(JColorManager::GetBlackColor(), f.GetStyle().color);
}

JTEST(AnalyzeWhitespaceDetection)
{
	JString buffer;
	bool useSpaces, isMixed;

	JSize tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 2, true, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "   abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	buffer   = "\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "    abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "   abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "   abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "   abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 2, true, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(3, tabWidth);

	buffer   = "   abcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 2, false, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(2, tabWidth);

	buffer   = "    abcd\n\tabcd\n\tabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "abc\n foobar\n\n\txyz\n\nxyz\n";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);
}

JTEST(AnalyzeWhitespaceWidth)
{
	JString buffer;
	bool useSpaces, isMixed;
	JSize tabWidth;

	buffer   = "   abcd\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	buffer   = "   abcd\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	buffer   = "abcd\n \nabcd\n\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "abcd\n \nabcd\n\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);

	buffer   = "abcd\n  \nabcd\n\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, true, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	buffer   = "abcd\n  \nabcd\n\nabcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 2,4
	buffer   = "  abcd\n  \nabcd\n\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 2,3,4
	buffer   = "  abcd\n  \nabcd\n   abcd\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 2,4,5
	buffer   = "  abcd\n  \nabcd\n     abcd\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 3,4
	buffer   = "   abcd\n   \nabcd\n\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,6
	buffer   = "   abcd\n   \nabcd\n\n      abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,6
	buffer   = "   abcd\n   \nabcd\n      abcd\n    abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,6,7
	buffer   = "   abcd\n   \nabcd\n      abcd\n    abcd\n       abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,6,8 - 3
	buffer   = "   abcd\n   \nabcd\n      abcd\n    abcd\n        abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,6,8 - 2
	buffer   = "   abcd\n    \nabcd\n      abcd\n    abcd\n        abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(2, tabWidth);

	// 3,4,8,9 - 3
	buffer   = "   abcd\n   \nabcd\n         abcd\n    abcd\n        abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(3, tabWidth);

	// 3,4,8,9 - 4
	buffer   = "   abcd\n    \nabcd\n         abcd\n    abcd\n        abcd";
	tabWidth = JAnalyzeWhitespace(buffer, 4, false, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);
	JAssertEqual(4, tabWidth);
}

struct WhitespaceData
{
	const JUtf8Byte*	fileName;
	const JSize			inputTabWidth;
	const bool			inputDefaultUseSpaces;
	const JSize			outputTabWidth;
	const bool			outputUseSpaces;
	const bool			outputIsMixed;
};

static const WhitespaceData kWhitespaceTestData[] =
{
{ "./data/test_whitespace.1.txt", 4, false, 2, true, false }
};

const int kWhitespaceTestDataCount = sizeof(kWhitespaceTestData) / sizeof(WhitespaceData);

JTEST(AnalyzeWhitespace)
{
	JString text;
	for (int i=0; i<kWhitespaceTestDataCount; i++)
	{
		JReadFile(JString(kWhitespaceTestData[i].fileName, JString::kNoCopy), &text);

		bool useSpaces, isMixed;
		const JSize tabWidth = JAnalyzeWhitespace(text,
			kWhitespaceTestData[i].inputTabWidth,
			kWhitespaceTestData[i].inputDefaultUseSpaces,
			&useSpaces, &isMixed);

		std::cout << kWhitespaceTestData[i].fileName << std::endl;
		JAssertEqual(kWhitespaceTestData[i].outputTabWidth, tabWidth);
		JAssertEqual(kWhitespaceTestData[i].outputUseSpaces, (bool) useSpaces);
		JAssertEqual(kWhitespaceTestData[i].outputIsMixed, (bool) isMixed);
	}
}

JTEST(Markdown)
{
	JStyledText st(false, true);

	JReadLimitedMarkdown(JString("hello", JString::kNoCopy), &st);
	JAssertStringsEqual("hello", st.GetText());

	JReadLimitedMarkdown(JString("hello *bold* -italic- _under_ `get_file_n-a-m-e` by*e*!", JString::kNoCopy), &st);
	JAssertStringsEqual("hello bold italic under get_file_n-a-m-e by*e*!", st.GetText());
//                              ^      ^     ^     ^               ^

	JRunArrayIterator<JFont> iter(st.GetStyles());
	JFont f;

	iter.MoveTo(kJIteratorStartBefore, 8);
	iter.Next(&f);
	JAssertTrue(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());

	iter.MoveTo(kJIteratorStartBefore, 15);
	iter.Next(&f);
	JAssertFalse(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());

	iter.MoveTo(kJIteratorStartBefore, 21);
	iter.Next(&f);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(1, f.GetStyle().underlineCount);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());

	iter.MoveTo(kJIteratorStartBefore, 27);
	iter.Next(&f);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertStringsEqual(JFontManager::GetDefaultMonospaceFontName(), f.GetName());

	iter.MoveTo(kJIteratorStartBefore, 42);
	iter.Next(&f);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);
	JAssertEqual(0, f.GetStyle().underlineCount);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());
/*
	JReadLimitedMarkdown(JString("_w*b*w_ _*b`t`b*_", JString::kNoCopy), &st);
	JAssertStringsEqual("wbw btb", st.GetText());

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	iter.Next(&f);
	JAssertFalse(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);

	iter.Next(&f);
	JAssertTrue(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);

	iter.Next(&f);
	JAssertFalse(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);

	iter.Next(&f);
	JAssertFalse(f.GetStyle().bold);
	JAssertFalse(f.GetStyle().italic);

	iter.Next(&f);
	JAssertTrue(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());

	iter.Next(&f);
	JAssertTrue(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);
	JAssertStringsEqual(JFontManager::GetDefaultMonospaceFontName(), f.GetName());

	iter.Next(&f);
	JAssertTrue(f.GetStyle().bold);
	JAssertTrue(f.GetStyle().italic);
	JAssertStringsEqual(JFontManager::GetDefaultFontName(), f.GetName());
*/
}
