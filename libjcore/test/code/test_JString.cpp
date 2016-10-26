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

int main()
{
	JUnitTestManager::Execute();
}

JTEST(Construction)
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

JTEST(IsValid)
{
	JAssertTrue(JString::IsValid(""));
	JAssertTrue(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94"));
	JAssertTrue(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(1,5)));
	JAssertFalse(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(11,13)));
}

JTEST(IntegerConversion)
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
	JAssertStringsEqual("0x2A", s5.GetBytes());

	JString s6(10, JString::kBase16, kJTrue);
	JAssertStringsEqual("0x0A", s6.GetBytes());
}

JTEST(FloatConversion)
{
	JString s1(42);
	JAssertStringsEqual("42", s1.GetBytes());

	JString s2(42.7);
	JAssertStringsEqual("42.7", s2.GetBytes());

	JString s3(-1.3e5);
	JAssertStringsEqual("-130000", s3.GetBytes());

	JString s4(1.3e20);
	JAssertStringsEqual("1.3e+20", s4.GetBytes());

	JString s5(1.57e5, 2, JString::kForceExponent, 2, 4);
	JAssertStringsEqual("1600.0000e+2", s5.GetBytes());
}

JTEST(ToLower)
{
	JString s;
	s.ToLower();
	JAssertStringsEqual("", s.GetBytes());

	s = "ABCD";
	s.ToLower();
	JAssertStringsEqual("abcd", s.GetBytes());

	s = "abcd";
	s.ToLower();
	JAssertStringsEqual("abcd", s.GetBytes());

	// ae

	s = "\xC3\x86";
	s.ToLower();
	JAssertStringsEqual("\xC3\xA6", s.GetBytes());

	// phi

	s = "\xCE\xA6";
	s.ToLower();
	JAssertStringsEqual("\xCF\x86", s.GetBytes());

	// sigma

	s = "\xCE\xA3";
	s.ToLower();
	JAssertStringsEqual("\xCF\x83", s.GetBytes());

	// greek

	s = "\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3";
	s.ToLower();
	JAssertStringsEqual("\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82", s.GetBytes());
}

JTEST(ToUpper)
{
	JString s;
	s.ToUpper();
	JAssertStringsEqual("", s.GetBytes());

	s = "abcd";
	s.ToUpper();
	JAssertStringsEqual("ABCD", s.GetBytes());

	s = "ABCD";
	s.ToUpper();
	JAssertStringsEqual("ABCD", s.GetBytes());

	// ae

	s = "\xC3\xA6";
	s.ToUpper();
	JAssertStringsEqual("\xC3\x86", s.GetBytes());

	// phi

	s = "\xCF\x86";
	s.ToUpper();
	JAssertStringsEqual("\xCE\xA6", s.GetBytes());

	// sigma

	s = "\xCF\x83";
	s.ToUpper();
	JAssertStringsEqual("\xCE\xA3", s.GetBytes());

	// double s

	s = "me\xC3\x9F";
	s.ToUpper();
	JAssertStringsEqual("MESS", s.GetBytes());

	// greek

	s = "\xCE\x9C\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s.ToUpper();
	JAssertStringsEqual("\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3", s.GetBytes());
}

JTEST(CopyBytes)
{
	const JIndex kStringLength = 10;
	JUtf8Byte string [ kStringLength ];

	const JUtf8Byte* stringList[] = {"", "1", "12345678", "123456789",
									 "1234567890", "123456789abcdef0"};
	const JSize kTestMax = sizeof(stringList) / sizeof(JUtf8Byte*);

	for (JIndex testnum=0; testnum<kTestMax; testnum++)
		{
		const JBoolean allCopied =
			JString::CopyBytes(stringList[testnum], kStringLength, string);

		JAssertEqualWithMessage(!strcmp(string, stringList[testnum]), allCopied, stringList[testnum]);
		JAssertEqual(0, JString::CompareMaxN(string, stringList[testnum], kStringLength-1));
		JAssertEqual(JMin(kStringLength-1, strlen(stringList[testnum])), strlen(string));
		}
}
