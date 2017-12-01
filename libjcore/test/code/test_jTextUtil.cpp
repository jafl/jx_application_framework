/******************************************************************************
 test_jTextUtil.cc

	Program to test jTextUtil.

	Written by John Lindal.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <jTextUtil.h>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(AnalyzeWhitespace)
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
}
