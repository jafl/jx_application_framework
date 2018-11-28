/******************************************************************************
 test_jTextUtil.cpp

	Program to test jTextUtil.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <jTextUtil.h>
#include <JFontManager.h>
#include <jFStreamUtil.h>
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

struct WhitespaceData
{
	const JUtf8Byte*	fileName;
	const JSize			inputTabWidth;
	const JBoolean		inputDefaultUseSpaces;
	const JSize			outputTabWidth;
	const JBoolean		outputUseSpaces;
	const JBoolean		outputIsMixed;
};

static const WhitespaceData kWhitespaceTestData[] =
{
	{ "./data/test_whitespace.1.txt", 4, kJFalse, 2, kJTrue, kJFalse }
};

const int kWhitespaceTestDataCount = sizeof(kWhitespaceTestData) / sizeof(WhitespaceData);

JTEST(AnalyzeWhitespace)
{
	JString text;
	for (int i=0; i<kWhitespaceTestDataCount; i++)
		{
		JReadFile(JString(kWhitespaceTestData[i].fileName, kJFalse), &text);

		JBoolean useSpaces, isMixed;
		const JSize tabWidth = JAnalyzeWhitespace(text,
			kWhitespaceTestData[i].inputTabWidth,
			kWhitespaceTestData[i].inputDefaultUseSpaces,
			&useSpaces, &isMixed);

		std::cout << kWhitespaceTestData[i].fileName << std::endl;
		JAssertEqual(kWhitespaceTestData[i].outputTabWidth, tabWidth);
		JAssertEqual(kWhitespaceTestData[i].outputUseSpaces, useSpaces);
		JAssertEqual(kWhitespaceTestData[i].outputIsMixed, isMixed);
		}
}

JTEST(Markdown)
{
	JStyledText st(kJFalse, kJTrue);

	JReadLimitedMarkdown(JString("hello", kJFalse), &st);
	JAssertStringsEqual("hello", st.GetText());

	JReadLimitedMarkdown(JString("hello *bold* -italic- _under_ `get_file_n-a-m-e` by*e*!", kJFalse), &st);
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
	JReadLimitedMarkdown(JString("_w*b*w_ _*b`t`b*_", kJFalse), &st);
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
