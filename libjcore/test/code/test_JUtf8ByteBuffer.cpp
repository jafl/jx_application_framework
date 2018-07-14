/******************************************************************************
 test_JUtf8ByteBuffer.cpp

	Program to test JUtf8ByteBuffer class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JUtf8ByteBuffer.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	JUtf8ByteBuffer buf(256);

	buf.Append("1234567890\xC2\xA9\xC3\x85\xC3", 15);
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85", buf.ExtractCharacters());
	JAssertFalse(buf.IsEmpty());

	buf.Append("\xA5\xE2\x9C\x94\n", 5);
	JAssertStringsEqual("\xC3\xA5\xE2\x9C\x94\n", buf.ExtractCharacters());
	JAssertTrue(buf.IsEmpty());

	std::cout << "expect invalid: a5" << std::endl;

	buf.Append("\xA5\xE2\x9C\x94\n", 5);
	JAssertStringsEqual("\xEF\xBF\xBD\xE2\x9C\x94\n", buf.ExtractCharacters());
	JAssertTrue(buf.IsEmpty());
}
