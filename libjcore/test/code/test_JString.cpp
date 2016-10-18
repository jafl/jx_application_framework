/******************************************************************************
 test_JString.cc

	Program to test JString class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JString.h>
#include <JMinMax.h>
#include <string>
#include <jassert_simple.h>

void TestConstruction()
{
	JString s1;
	JAssertStringsEqual("", s1.GetBytes());

	JString s2("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	JAssertEqual(19, s2.GetByteCount());
	JAssertEqual(14, s2.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s2.GetBytes());

	JString s3("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 14);
	JAssertEqual(14, s3.GetByteCount());
	JAssertEqual(12, s3.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85", s3.GetBytes());

	JString s4("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(2,5));
	JAssertEqual(4, s4.GetByteCount());
	JAssertEqual(4, s4.GetCharacterCount());
	JAssertStringsEqual("2345", s4.GetBytes());

	JString s5("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(8,14));
	JAssertEqual(7, s5.GetByteCount());
	JAssertEqual(5, s5.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s5.GetBytes());

	JString s6(s2);
	JAssertEqual(19, s6.GetByteCount());
	JAssertEqual(14, s6.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s6.GetBytes());

	JString s7(s2, JCharacterRange(2,5));
	JAssertEqual(4, s7.GetByteCount());
	JAssertEqual(4, s7.GetCharacterCount());
	JAssertStringsEqual("2345", s7.GetBytes());

	std::string ss1("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");

	JString s8(ss1);
	JAssertEqual(19, s8.GetByteCount());
	JAssertEqual(14, s8.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s8.GetBytes());

	JString s9(ss1, JUtf8ByteRange(8,14));
	JAssertEqual(7, s9.GetByteCount());
	JAssertEqual(5, s9.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s9.GetBytes());
}

void TestIntegerConversion()
{
	JString s1(42, JString::kBase10);
	JAssertStringsEqual("42", s1.GetBytes());

	JString s2(42, JString::kBase2);
	JAssertStringsEqual("101010", s2.GetBytes());

	JString s3(42, JString::kBase2, kJTrue);
	JAssertStringsEqual("00101010", s3.GetBytes());

	JString s4(42, JString::kBase8);
	JAssertStringsEqual("52", s4.GetBytes());

	JString s5(42, JString::kBase16);
	JAssertStringsEqual("0x2a", s5.GetBytes());
}

void TestFloatConversion()
{
	JString s1(42);
	JAssertStringsEqual("42", s1.GetBytes());

	JString s2(42.7);
	JAssertStringsEqual("42.7", s2.GetBytes());

	JString s3(-1.3e5);
	JAssertStringsEqual("-130000", s3.GetBytes());

	JString s4(1.3e20);
	JAssertStringsEqual("1.3e+20", s4.GetBytes());
}

void
TestCopyMaxN()
{
	const JIndex kStringLength = 10;
	JUtf8Byte string [ kStringLength ];

	const JUtf8Byte* stringList[] = {"", "1", "12345678", "123456789",
									 "1234567890", "123456789abcdef0"};
	const JSize kTestMax = sizeof(stringList) / sizeof(JUtf8Byte*);

	for (JIndex testnum=0; testnum<kTestMax; testnum++)
		{
		const JBoolean allCopied =
			JString::CopyMaxN(stringList[testnum], kStringLength, string);

		JAssertEqual(!strcmp(string, stringList[testnum]), allCopied);
		JAssertEqual(0, JString::CompareMaxN(string, stringList[testnum], kStringLength-1));
		JAssertEqual(JMin(kStringLength-1, strlen(stringList[testnum])), strlen(string));
		}
}

static const JUnitTest tests[] =
{
	TestConstruction,
	TestIntegerConversion,
	TestFloatConversion,
	TestCopyMaxN,
	NULL
};

int main()
{
	JUnitTestManager::Execute(tests);
}
