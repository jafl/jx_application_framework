/******************************************************************************
 test_jStreamUtil.cpp

	Program to test stream utilities.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(JReadAll)
{
	std::ifstream input("data/test_JReadAll.txt");
	JAssertTrue(input.good());

	JString s;
	JReadAll(input, &s);
	JAssertStringsEqual("This is \xE2\x9C\x94 test", s);

	int fd = open("data/test_JReadAll.txt", O_RDONLY);
	JReadAll(fd, &s);
	JAssertStringsEqual("This is \xE2\x9C\x94 test", s);
}

JTEST(JReadLine)	// also tests JReadUntil
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JString s = JReadLine(input);
	JAssertStringsEqual("This is \xE2\x9C\x94 test", s);

	bool foundNewLine;
	s = JReadLine(input, &foundNewLine);
	JAssertTrue(foundNewLine);
	JAssertStringsEqual("This is another test", s);

	s = JReadLine(input);
	JAssertEqual(1100, s.GetCharacterCount());
}

JTEST(JReadUntil)
{
	int fd = open("data/test_JReadLine.txt", O_RDONLY);

	bool foundDelimiter;
	JString s = JReadUntil(fd, ' ', &foundDelimiter);
	JAssertStringsEqual("This", s);
	JAssertTrue(foundDelimiter);

	s = JReadUntil(fd, '\n', &foundDelimiter);
	JAssertStringsEqual("is \xE2\x9C\x94 test", s);
	JAssertTrue(foundDelimiter);

	close(fd);
}

JTEST(JReadUntilws)
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JString s = JReadUntilws(input);
	JAssertStringsEqual("This", s);

	bool foundws;
	s = JReadUntilws(input, &foundws);
	JAssertTrue(foundws);
	JAssertStringsEqual("is", s);

	s = JReadUntilws(input);
	JAssertStringsEqual("\xE2\x9C\x94", s);

	s = JReadUntilws(input);
	JAssertStringsEqual("test", s);
}

JTEST(JIgnoreLine)
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JIgnoreLine(input);

	bool foundNewLine;
	JString s = JReadLine(input, &foundNewLine);
	JAssertStringsEqual("This is another test", s);
	JAssertTrue(foundNewLine);
}

JTEST(JIgnoreUntil1)
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JIgnoreUntil(input, ' ');

	JString s = JReadLine(input);
	JAssertStringsEqual("is \xE2\x9C\x94 test", s);
}

JTEST(JIgnoreUntilN)
{
	std::ifstream input("data/test_JReadLine.txt");
	JAssertTrue(input.good());

	JIgnoreUntil(input, "\xE2\x9C\x94");

	JString s = JReadLine(input);
	JAssertStringsEqual(" test", s);
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

	JAssertStringsEqual(s, output2.str().c_str());
}
