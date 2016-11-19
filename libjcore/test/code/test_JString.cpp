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

	JString s2;
	s2.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	JAssertEqual(19, s2.GetByteCount());
	JAssertEqual(14, s2.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s2.GetBytes());

	JString s3;
	s3.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 14);
	JAssertEqual(14, s3.GetByteCount());
	JAssertEqual(12, s3.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85", s3.GetBytes());

	JString s4;
	s4.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(2,5));
	JAssertEqual(4, s4.GetByteCount());
	JAssertEqual(4, s4.GetCharacterCount());
	JAssertStringsEqual("2345", s4.GetBytes());

	JString s5;
	s5.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(8,14));
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

	JString s8;
	s8.Set(ss1);
	JAssertEqual(19, s8.GetByteCount());
	JAssertEqual(14, s8.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s8.GetBytes());

	JString s9(ss1, JUtf8ByteRange(8,14));
	JAssertEqual(7, s9.GetByteCount());
	JAssertEqual(5, s9.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s9.GetBytes());
}

JTEST(Set)
{
	JString s;

	s.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	JAssertEqual(19, s.GetByteCount());
	JAssertEqual(14, s.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s.GetBytes());

	s.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 14);
	JAssertEqual(14, s.GetByteCount());
	JAssertEqual(12, s.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85", s.GetBytes());

	s.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(2,5));
	JAssertEqual(4, s.GetByteCount());
	JAssertEqual(4, s.GetCharacterCount());
	JAssertStringsEqual("2345", s.GetBytes());

	s.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(8,14));
	JAssertEqual(7, s.GetByteCount());
	JAssertEqual(5, s.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s.GetBytes());

	JString s2;
	s2.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	s.Set(s2);
	JAssertEqual(19, s.GetByteCount());
	JAssertEqual(14, s.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s.GetBytes());

	s.Set(s2, JCharacterRange(2,5));
	JAssertEqual(4, s.GetByteCount());
	JAssertEqual(4, s.GetCharacterCount());
	JAssertStringsEqual("2345", s.GetBytes());

	std::string ss1("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");

	s.Set(ss1);
	JAssertEqual(19, s.GetByteCount());
	JAssertEqual(14, s.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s.GetBytes());

	s.Set(ss1, JUtf8ByteRange(8,14));
	JAssertEqual(7, s.GetByteCount());
	JAssertEqual(5, s.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s.GetBytes());

	s2 = "890\xC2\xA9\xC3\x85";
	s  = s2;
	JAssertEqual(7, s.GetByteCount());
	JAssertEqual(5, s.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s.GetBytes());
	JAssertEqual(s, s2);

	s = "1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94";
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s.GetBytes());

	std::string ss2 = "\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s = ss2;
	JAssertStringsEqual("\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82", s.GetBytes());

	JAssertFalse(s.IsEmpty());

	s.Clear();
	JAssertEqual(0, s.GetByteCount());
	JAssertEqual(0, s.GetCharacterCount());
	JAssertStringsEqual("", s.GetBytes());
	JAssertTrue(s.IsEmpty());
}

JTEST(IsValid)
{
	JAssertTrue(JString::IsValid(""));
	JAssertTrue(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94"));
	JAssertTrue(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(1,5)));
	JAssertTrue(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(10,12)));
	JAssertFalse(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(11,13)));

	JString s;
	s.Set("\xC3\xA6" "34567\xCE\xA6" "90\xE2\x9C\x94\xCE\xA6");

	JAssertFalse(s.CharacterIndexValid(0));
	JAssertTrue(s.CharacterIndexValid(2));
	JAssertTrue(s.CharacterIndexValid(11));
	JAssertFalse(s.CharacterIndexValid(12));

	JAssertFalse(s.ByteIndexValid(0));
	JAssertTrue(s.ByteIndexValid(2));
	JAssertTrue(s.ByteIndexValid(12));
	JAssertTrue(s.ByteIndexValid(16));
	JAssertFalse(s.ByteIndexValid(17));

	JAssertFalse(s.RangeValid(JCharacterRange()));
	JAssertTrue(s.RangeValid(JCharacterRange(2,11)));
	JAssertFalse(s.RangeValid(JCharacterRange(2,12)));

	JAssertFalse(s.RangeValid(JUtf8ByteRange()));
	JAssertTrue(s.RangeValid(JUtf8ByteRange(2,11)));
	JAssertTrue(s.RangeValid(JUtf8ByteRange(2,12)));
	JAssertTrue(s.RangeValid(JUtf8ByteRange(2,16)));
	JAssertFalse(s.RangeValid(JUtf8ByteRange(2,17)));
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

	JString s5(1.57e5, 2, JString::kUseGivenExponent, 2, 2);
	JAssertStringsEqual("1600.00e+2", s5.GetBytes());

	JString s6(1.57e5, 1, JString::kForceExponent);
	JAssertStringsEqual("1.6e+5", s6.GetBytes());

	JString s7(1.57e5, 1, JString::kForceNoExponent);
	JAssertStringsEqual("157000.0", s7.GetBytes());

	JString s8(1.57e5, JString::kPrecisionAsNeeded, JString::kForceNoExponent, 0, 2);
	JAssertStringsEqual("160000", s8.GetBytes());

	JString s9(1, JString::kPrecisionAsNeeded, JString::kUseGivenExponent, -3);
	JAssertStringsEqual("1000e-3", s9.GetBytes());

	JString s10(1, JString::kPrecisionAsNeeded, JString::kUseGivenExponent, +2);
	JAssertStringsEqual("0.01e+2", s10.GetBytes());

	JString s11(537.6, 2, JString::kForceExponent);
	JAssertStringsEqual("5.38e+2", s11.GetBytes());

	JString s12(5.374, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	JAssertStringsEqual("5.37", s12.GetBytes());

	JString s13(5.375, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	JAssertStringsEqual("5.38", s13.GetBytes());

	JString s14(5.376, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	JAssertStringsEqual("5.38", s14.GetBytes());

	JString s15(9.995, 2);
	JAssertStringsEqual("10.00", s15.GetBytes());
}

JTEST(Concatenate)
{
	JString s("5", 1);

	s.Prepend("");
	s.Append("");
	JAssertStringsEqual("5", s.GetBytes());

	s.Prepend("4");
	JAssertStringsEqual("45", s.GetBytes());

	s.Append("6");
	JAssertStringsEqual("456", s.GetBytes());

	s.Prepend("30", 1);
	JAssertStringsEqual("3456", s.GetBytes());

	s.Append("70", 1);
	JAssertStringsEqual("34567", s.GetBytes());

	s.Prepend(JUtf8Character("\xC3\xA6"));
	JAssertStringsEqual("\xC3\xA6" "34567", s.GetBytes());

	s.Append(JUtf8Character("\xCE\xA6"));
	JAssertStringsEqual("\xC3\xA6" "34567\xCE\xA6", s.GetBytes());

	s += "90\xE2\x9C\x94";
	JAssertStringsEqual("\xC3\xA6" "34567\xCE\xA6" "90\xE2\x9C\x94", s.GetBytes());

	s += JUtf8Character("\xCE\xA6");
	JAssertStringsEqual("\xC3\xA6" "34567\xCE\xA6" "90\xE2\x9C\x94\xCE\xA6", s.GetBytes());
}

JTEST(Get)
{
	JString s;
	s.Set("C");
	JAssertEqual('C', s.GetFirstCharacter());
	JAssertEqual('C', s.GetLastCharacter());

	s = "123";
	JAssertEqual('1', s.GetFirstCharacter());
	JAssertEqual('3', s.GetLastCharacter());

	s = "\xC3\xA6" "34567\xCE\xA6" "90\xCE\xA6\xE2\x9C\x94";
	JAssertEqual(JUtf8Character("\xC3\xA6"), s.GetFirstCharacter());
	JAssertEqual(JUtf8Character("\xE2\x9C\x94"), s.GetLastCharacter());

	JUtf8Byte* s1 = s.AllocateBytes();
	JAssertStringsEqual(s.GetBytes(), s1);
	delete [] s1;
}

JTEST(Contains)
{
	JString s;
	JAssertFalse(s.BeginsWith("1"));
	JAssertFalse(s.Contains("1"));
	JAssertFalse(s.EndsWith("1"));

	s = "abcd";

	JAssertTrue(s.BeginsWith("ab"));
	JAssertTrue(s.BeginsWith("AB", kJFalse));
	JAssertFalse(s.BeginsWith("bc"));
	JAssertFalse(s.BeginsWith("BC", kJFalse));

	JAssertTrue(s.Contains("ab"));
	JAssertTrue(s.Contains("AB", kJFalse));
	JAssertTrue(s.Contains("bc"));
	JAssertTrue(s.Contains("BC", kJFalse));
	JAssertTrue(s.Contains("cd"));
	JAssertTrue(s.Contains("CD", kJFalse));
	JAssertFalse(s.Contains("abcdefg"));
	JAssertFalse(s.Contains("123"));

	JAssertTrue(s.EndsWith("cd"));
	JAssertTrue(s.EndsWith("CD", kJFalse));
	JAssertFalse(s.EndsWith("bc"));
	JAssertFalse(s.EndsWith("BC", kJFalse));

	s = "\xC3\x86" "a34567\xCE\xA6" "90b\xE2\x9C\x94\xCE\xA6";

	JAssertTrue(s.BeginsWith("\xC3\x86" "a34"));
	JAssertTrue(s.BeginsWith("\xC3\xA6" "A34", kJFalse));

	JAssertTrue(s.Contains("\xCE\xA6" "90b"));
	JAssertTrue(s.Contains("\xCF\x86" "90B", kJFalse));

	JAssertTrue(s.EndsWith("b\xE2\x9C\x94\xCE\xA6"));
	JAssertTrue(s.EndsWith("B\xE2\x9C\x94\xCF\x86", kJFalse));
}

JTEST(Whitespace)
{
	JString s;
	s.TrimWhitespace();
	JAssertStringsEqual("", s.GetBytes());

	s.Set("\t  foo\n");
	s.TrimWhitespace();
	JAssertStringsEqual("foo", s.GetBytes());
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

	// ae phi sigma

	s = "\xC3\x86\xCE\xA6\xCE\xA3";
	s.ToLower();
	JAssertStringsEqual("\xC3\xA6\xCF\x86\xCF\x82", s.GetBytes());

	s = "\xC3\xA6\xCF\x86\xCF\x82";
	s.ToLower();
	JAssertStringsEqual("\xC3\xA6\xCF\x86\xCF\x82", s.GetBytes());

	s = "\xC3\x86 \xCE\xA6 \xCE\xA3";
	s.ToLower();
	JAssertStringsEqual("\xC3\xA6 \xCF\x86 \xCF\x83", s.GetBytes());

	// greek

	s = "\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3";
	s.ToLower();
	JAssertStringsEqual("\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82", s.GetBytes());

	s = "\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
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

	// ae phi sigma double-s

	s = "\xC3\xA6\xCF\x86\xCF\x83" "me\xC3\x9F";
	s.ToUpper();
	JAssertStringsEqual("\xC3\x86\xCE\xA6\xCE\xA3" "MESS", s.GetBytes());

	s = "\xC3\x86\xCE\xA6\xCE\xA3" "MESS";
	s.ToUpper();
	JAssertStringsEqual("\xC3\x86\xCE\xA6\xCE\xA3" "MESS", s.GetBytes());

	// greek

	s = "\xCE\x9C\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s.ToUpper();
	JAssertStringsEqual("\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3", s.GetBytes());

	s = "\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3";
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
		JAssertEqual(0, JString::CompareMaxN(string, stringList[testnum], JMin(strlen(stringList[testnum]), kStringLength-1)));
		JAssertEqual(JMin(kStringLength-1, strlen(stringList[testnum])), strlen(string));
		}
}

JTEST(streaming)
{
	JString s1;
	s1.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");

	std::ostringstream out;
	out << s1;

	const std::string data = out.str();
	std::istringstream in(data.c_str(), data.length());

	JString s2;
	in >> s2;

	JAssertEqual(s1, s2);
}
