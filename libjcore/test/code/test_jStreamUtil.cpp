/******************************************************************************
 test_jStreamUtil.cc

	Program to test stream utilities.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <jStreamUtil.h>
#include <fstream>
#include <sstream>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(JReadAll)
{
	std::ifstream input("data/test_JReadAll.txt");
	JAssertTrue(input.good());

	JString s;
	JReadAll(input, &s);
	JAssertStringsEqual("This is \xE2\x9C\x94 test", s);
}

JTEST(JReadLine)	// also tests JReadUntil
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JString s = JReadLine(input);
	JAssertStringsEqual("This is \xE2\x9C\x94 test", s);

	JBoolean foundNewLine;
	s = JReadLine(input, &foundNewLine);
	JAssertTrue(foundNewLine);
	JAssertStringsEqual("This is another test");

	s = JReadLine(input);
	JAssertEqual(1100, s.GetCharacterCount());
}

JTEST(JReadUntilws)
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JString s = JReadUntilws(input);
	JAssertStringsEqual("This", s);

	JBoolean foundws;
	s = JReadLine(input, &foundws);
	JAssertTrue(foundws);
	JAssertStringsEqual("is");

	s = JReadLine(input);
	JAssertStringsEqual("\xE2\x9C\x94");

	s = JReadLine(input);
	JAssertStringsEqual("test");
}

JTEST(JIgnoreLine)
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JIgnoreLine(input);

	JString s = JReadLine(input, &foundNewLine);
	JAssertStringsEqual("This is another test");
}

JTEST(JIgnoreUntil1)
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JIgnoreUntil(input, ' ');

	JString s = JReadLine(input);
	JAssertStringsEqual("is \xE2\x9C\x94 test");
}

JTEST(JIgnoreUntilN)
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JIgnoreUntil(input, "\xE2\x9C\x94");

	JString s = JReadLine(input);
	JAssertStringsEqual(" test");
}

JTEST(Base64)
{
	const JUtf8Byte* s = "The quick fox jumped over the lazy dog.";

	std::istringstream input1(s);
	std::ostringstream output1;
	JEncodeBase64(input1, output1);

	std::istringstream input2(output1.str());
	std::ostringstream output2;
	JDecodeBase64(input2, output2);

	JAssertStringsEqual(s, output2.str());
}
