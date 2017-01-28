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

	JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);

	buffer = "    abcd";
	JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);

	buffer = "    abcd";
	JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);

	buffer = "    abcd";
	JAnalyzeWhitespace(buffer, 2, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);

	buffer = "   abcd";
	JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertFalse(isMixed);

	buffer = "\tabcd";
	JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);

	buffer = "    abcd\n\tabcd";
	JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertTrue(isMixed);

	buffer = "    abcd\n\tabcd";
	JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);

	buffer = "   abcd\n\tabcd";
	JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);

	buffer = "   abcd\n\tabcd";
	JAnalyzeWhitespace(buffer, 4, kJFalse, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertFalse(isMixed);

	buffer = "   abcd\n\tabcd";
	JAnalyzeWhitespace(buffer, 2, kJTrue, &useSpaces, &isMixed);
	JAssertTrue(useSpaces);
	JAssertTrue(isMixed);

	buffer = "   abcd\n\tabcd";
	JAnalyzeWhitespace(buffer, 2, kJFalse, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);

	buffer = "    abcd\n\tabcd\n\tabcd";
	JAnalyzeWhitespace(buffer, 4, kJTrue, &useSpaces, &isMixed);
	JAssertFalse(useSpaces);
	JAssertTrue(isMixed);
}
