/******************************************************************************
 test_JString.cpp

	Program to test JString class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "TestString.h"
#include "JPtrArray-JString.h"
#include "JMinMax.h"
#include <fstream>
#include <string>
#include <locale.h>
#include <stdlib.h>
#include "jAssert.h"

int main()
{
	JString::theDefaultMinLgSize = 2; // Ridiculously small to really exercise resizer

	std::cout << "Current locale: " << setlocale(LC_ALL, "") << std::endl;
	return JTestManager::Execute();
}

JTEST(Counting)
{
	JAssertEqual( 5, JString::CountCharacters("12345"));
	JAssertEqual(14, JString::CountCharacters("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94"));
	JAssertEqual(11, JString::CountCharacters("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 12));
	JAssertEqual( 2, JString::CountCharacters("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(13, 16)));

	JAssertEqual( 4, JString::CountBytes("12345", 4));
	JAssertEqual(14, JString::CountBytes("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 12));

	JSize byteCount;
	bool ok = JString::CountBytesBackward("12345", 5, 4, &byteCount);
	JAssertTrue(ok);
	JAssertEqual(4, byteCount);

	ok = JString::CountBytesBackward("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 19, 7, &byteCount);
	JAssertTrue(ok);
	JAssertEqual(12, byteCount);

	ok = JString::CountBytesBackward("12345", 2, 4, &byteCount);
	JAssertFalse(ok);
	JAssertEqual(2, byteCount);

	ok = JString::CountBytesBackward("12345", 0, 4, &byteCount);
	JAssertFalse(ok);
	JAssertEqual(0, byteCount);

	JAssertEqual(JUtf8ByteRange(2, 5),
		JString::CharacterToUtf8ByteRange("12345",
			JCharacterRange(2, 5)));

	JAssertEqual(JUtf8ByteRange(8, 14),
		JString::CharacterToUtf8ByteRange("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94",
			JCharacterRange(8, 12)));
}

JTEST(Construction)
{
	JString s1;
	JAssertStringsEqual("", s1);

	JAssertTrue(JString::IsEmpty((JUtf8Byte*) nullptr));
	JAssertTrue(JString::IsEmpty(""));
	JAssertTrue(s1.IsEmpty());
	JAssertTrue(JString::IsEmpty((JString*) nullptr));
	JAssertTrue(JString::IsEmpty(&s1));

	JString s2("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	JAssertEqual(19, s2.GetByteCount());
	JAssertEqual(14, s2.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s2);

	JString s3("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 14);
	JAssertEqual(14, s3.GetByteCount());
	JAssertEqual(12, s3.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85", s3);

	JString s4("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(2,5));
	JAssertEqual(4, s4.GetByteCount());
	JAssertEqual(4, s4.GetCharacterCount());
	JAssertStringsEqual("2345", s4);

	JString s5("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(8,14));
	JAssertEqual(7, s5.GetByteCount());
	JAssertEqual(5, s5.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s5);

	JString s12("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange());
	JAssertTrue(s12.IsEmpty());

	JString s6(s2);
	JAssertEqual(19, s6.GetByteCount());
	JAssertEqual(14, s6.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s6);

	JString s7(s2, JCharacterRange(2,5));
	JAssertEqual(4, s7.GetByteCount());
	JAssertEqual(4, s7.GetCharacterCount());
	JAssertStringsEqual("2345", s7);

	std::string ss1("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");

	JString s14(ss1);
	JAssertEqual(19, s14.GetByteCount());
	JAssertEqual(14, s14.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s14);

	JString s8(ss1, JUtf8ByteRange(1, 19));
	JAssertEqual(19, s8.GetByteCount());
	JAssertEqual(14, s8.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s8);

	s8.Set("foo");
	JAssertStringsEqual("foo", s8);

	JString s10(s2, JCharacterRange(10,12));
	JAssertEqual(5, s10.GetByteCount());
	JAssertEqual(3, s10.GetCharacterCount());
	JAssertStringsEqual("0\xC2\xA9\xC3\x85", s10);

	JString s11(s2, JCharacterRange(12,14));
	JAssertEqual(7, s11.GetByteCount());
	JAssertEqual(3, s11.GetCharacterCount());
	JAssertStringsEqual("\xC3\x85\xC3\xA5\xE2\x9C\x94", s11);

	JString s13;
	s13.Set(s2, JCharacterRange(12,14));
	JAssertEqual(7, s13.GetByteCount());
	JAssertEqual(3, s13.GetCharacterCount());
	JAssertStringsEqual("\xC3\x85\xC3\xA5\xE2\x9C\x94", s13);

	JString s15(JUtf8Character("\xC3\x85"));
	JAssertEqual(2, s15.GetByteCount());
	JAssertStringsEqual("\xC3\x85", s15);

	JString s16(nullptr, 0);
	JAssertStringsEqual("", s16);

	s16.Set("\xA7", 0);
	JAssertStringsEqual("", s16);
}

JTEST(LazyConstruction)
{
	JString s2("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JString::kNoCopy);
	JAssertFalse(s2.IsOwner());
	JAssertEqual(19, s2.GetByteCount());
	JAssertEqual(14, s2.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s2);

	JString s3("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 14, JString::kNoCopy);
	JAssertFalse(s3.IsOwner());
	JAssertEqual(14, s3.GetByteCount());
	JAssertEqual(12, s3.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85", s3);
	s3.Set("abc");		// test replace
	JAssertStringsEqual("abc", s3);

	JString s4("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(2,5), JString::kNoCopy);
	JAssertFalse(s4.IsOwner());
	JAssertEqual(4, s4.GetByteCount());
	JAssertEqual(4, s4.GetCharacterCount());
	JAssertStringsEqual("2345", s4);
	JAssertStringsEqual("2345", s4.GetBytes());		// test null termination; ignore memory leak

	JString s5("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(8,14), JString::kNoCopy);
	JAssertFalse(s5.IsOwner());
	JAssertEqual(7, s5.GetByteCount());
	JAssertEqual(5, s5.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s5);
	s5.Append("abc");		// test modify
	JAssertEqual(10, s5.GetByteCount());
	JAssertEqual(8, s5.GetCharacterCount());

	JString s12("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(), JString::kNoCopy);
	JAssertFalse(s12.IsOwner());
	JAssertTrue(s12.IsEmpty());

	JString s13("ab 90\xC2\xA9 58", JUtf8ByteRange(3, 8), JString::kNoCopy);
	JAssertFalse(s13.IsOwner());
	JAssertEqual(5, s13.GetCharacterCount());
	s13.TrimWhitespace();	// test modify
	JAssertStringsEqual("90\xC2\xA9", s13);

	JString s14("\xC3\x86\xCE\xA6\xCE\xA3", 0);
	JAssertTrue(s14.IsOwner());
	JString s15(s14.GetBytes(), JString::kNoCopy);
	JAssertFalse(s15.IsOwner());
	s15.ToLower();
	JAssertStringsEqual("\xC3\x86\xCE\xA6\xCE\xA3", s14);
	JAssertStringsEqual("\xC3\xA6\xCF\x86\xCF\x82", s15);

	// test null terminator detection

	const JUtf8Byte* const_s = "abcdefg";

	JString s16(const_s, 3, JString::kNoCopy);
	JAssertFalse(s16.IsOwner());
	JAssertStringsEqual("abc", s16.GetBytes());
	JAssertTrue(s16.IsOwner());

	JString s17(const_s, JUtf8ByteRange(5, 7), JString::kNoCopy);
	JAssertFalse(s17.IsOwner());
	JAssertStringsEqual("efg", s17.GetBytes());
	JAssertFalse(s17.IsOwner());
}

JString TestNameReturnValueOptimization()
{
	return JString("fleem", 4, JString::kNoCopy);
}

JTEST(Allocation)
{
	JString s0 = JString("foobar", 3, JString::kNoCopy);

	const JUtf8Byte* p1 = s0.GetRawBytes();
	const JUtf8Byte* p2 = s0.GetBytes();
	JAssertFalse(((void*) p1) == ((void*) p2));
	JAssertEqual(3, s0.GetCharacterCount());

	auto* s = jnew JString("foobar", 3, JString::kNoCopy);

	p1 = s->GetRawBytes();
	p2 = s->GetBytes();
	JAssertEqual((void*) p1, (void*) p2);
	JAssertEqual(3, s->GetCharacterCount());

	srand(time(NULL));
	if (rand() < RAND_MAX/2)	// try to force processor to pre-execute 2 branches
	{
		auto* s1 = jnew JString(TestNameReturnValueOptimization());
		assert( s1 != nullptr );

		auto* s2 = new JString(TestNameReturnValueOptimization());
		assert( s2 != nullptr );

		p1 = s1->GetRawBytes();
		p2 = s1->GetBytes();
		JAssertEqual((void*) p1, (void*) p2);
		JAssertEqual(4, s1->GetCharacterCount());

		p1 = s2->GetRawBytes();
		p2 = s2->GetBytes();
		JAssertEqual((void*) p1, (void*) p2);
		JAssertEqual(4, s2->GetCharacterCount());
	}
	else
	{
		auto* s3 = jnew JString(TestNameReturnValueOptimization());
		assert( s3 != nullptr );

		auto* s4 = new JString(TestNameReturnValueOptimization());
		assert( s4 != nullptr );

		p1 = s3->GetRawBytes();
		p2 = s3->GetBytes();
		JAssertEqual((void*) p1, (void*) p2);
		JAssertEqual(4, s3->GetCharacterCount());

		p1 = s4->GetRawBytes();
		p2 = s4->GetBytes();
		JAssertEqual((void*) p1, (void*) p2);
		JAssertEqual(4, s4->GetCharacterCount());
	}
}

JTEST(Set)
{
	JString s("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JAssertEqual(19, s.GetByteCount());
	JAssertEqual(14, s.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s);

	s.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 14);
	JAssertEqual(14, s.GetByteCount());
	JAssertEqual(12, s.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85", s);

	s.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(2,5));
	JAssertEqual(4, s.GetByteCount());
	JAssertEqual(4, s.GetCharacterCount());
	JAssertStringsEqual("2345", s);

	s.Set("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(8,14));
	JAssertEqual(7, s.GetByteCount());
	JAssertEqual(5, s.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s);

	JString s2("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	s.Set(s2);
	JAssertEqual(19, s.GetByteCount());
	JAssertEqual(14, s.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s);

	s.Set(s2, JCharacterRange(2,5));
	JAssertEqual(4, s.GetByteCount());
	JAssertEqual(4, s.GetCharacterCount());
	JAssertStringsEqual("2345", s);

	std::string ss1("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");

	s.Set(ss1);
	JAssertEqual(19, s.GetByteCount());
	JAssertEqual(14, s.GetCharacterCount());
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s);

	s.Set(ss1, JUtf8ByteRange(8,14));
	JAssertEqual(7, s.GetByteCount());
	JAssertEqual(5, s.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s);

	s2 = "890\xC2\xA9\xC3\x85";
	s  = s2;
	JAssertEqual(7, s.GetByteCount());
	JAssertEqual(5, s.GetCharacterCount());
	JAssertStringsEqual("890\xC2\xA9\xC3\x85", s);
	JAssertEqual(s, s2);

	s = "1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94";
	JAssertStringsEqual("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", s);

	std::string ss2 = "\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s = ss2;
	JAssertStringsEqual("\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82", s);

	JAssertFalse(s.IsEmpty());

	s.Clear();
	JAssertEqual(0, s.GetByteCount());
	JAssertEqual(0, s.GetCharacterCount());
	JAssertStringsEqual("", s);
	JAssertTrue(s.IsEmpty());

	// test assignment to part of self

	s = "890\xC2\xA9\xC3\x85";
	JString s3(s.GetBytes(), 5, JString::kNoCopy);
	s = s3;
	JAssertStringsEqual("890\xC2\xA9", s);

	s = "890\xC2\xA9\xC3\x85";
	JString s4(s.GetBytes(), JUtf8ByteRange(2, 5), JString::kNoCopy);
	s = s4;
	JAssertStringsEqual("90\xC2\xA9", s);
}

JTEST(IsValid)
{
	JAssertTrue(JString::IsValid(""));
	JAssertTrue(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94"));
	JAssertTrue(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(1,5)));
	JAssertTrue(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(10,12)));
	JAssertFalse(JString::IsValid("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", JUtf8ByteRange(11,13)));

	JString s("\xC3\xA6" "34567\xCE\xA6" "90\xE2\x9C\x94\xCE\xA6", 0);

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
	JString s1(42);
	JAssertStringsEqual("42", s1);
	JAssertTrue(s1.IsFloat());
	JAssertTrue(s1.IsInteger());
	JAssertTrue(s1.IsUInt());
	JAssertFalse(s1.IsHexValue());

	JFloat f;
	JAssertTrue(s1.ConvertToFloat(&f));
	JAssertEqual(42.0, f);

	JInteger i;
	JAssertTrue(s1.ConvertToInteger(&i));
	JAssertEqual(42, i);

	JUInt u;
	JAssertTrue(s1.ConvertToUInt(&u));
	JAssertEqual(42, u);

	JString s2(42UL, JString::kBase2);
	JAssertStringsEqual("101010", s2);

	JString s3(42UL, JString::kBase2, true);
	JAssertStringsEqual("00101010", s3);

	JString s4(42UL, JString::kBase8);
	JAssertStringsEqual("52", s4);

	JString s5(42UL, JString::kBase16);
	JAssertStringsEqual("0x2A", s5);
	JAssertTrue(s5.IsFloat());
	JAssertTrue(s5.IsInteger());
	JAssertTrue(s5.IsUInt());
	JAssertTrue(s5.IsHexValue());

	JAssertTrue(s5.ConvertToFloat(&f));
	JAssertEqual(42.0, f);

	JAssertTrue(s5.ConvertToInteger(&i));
	JAssertEqual(42, i);

	JAssertTrue(s5.ConvertToUInt(&u));
	JAssertEqual(42, u);

	JString s6(10UL, JString::kBase16, true);
	JAssertStringsEqual("0x0A", s6);
}

JTEST(FloatConversion)
{
	JString s1(42);
	JAssertStringsEqual("42", s1);

	JString s2(42.7);
	JAssertStringsEqual("42.7", s2);

	JFloat f;
	JAssertTrue(s2.ConvertToFloat(&f));
	JAssertEqual(42.7, f);

	JInteger i;
	JAssertFalse(s2.ConvertToInteger(&i));

	JUInt u;
	JAssertFalse(s2.ConvertToUInt(&u));

	JString s3(-1.3e5);
	JAssertStringsEqual("-130000", s3);

	JAssertTrue(s3.ConvertToFloat(&f));
	JAssertEqual(-1.3e5, f);

	JAssertTrue(s3.ConvertToInteger(&i));
	JAssertEqual(-130000, i);

	JAssertFalse(s3.ConvertToUInt(&u));

	JString s4(1.3e20);
	JAssertStringsEqual("1.3e+20", s4);

	JString s5(1.57e5, 2, JString::kUseGivenExponent, 2, 2);
	JAssertStringsEqual("1600.00e+2", s5);

	JString s6(1.57e5, 1, JString::kForceExponent);
	JAssertStringsEqual("1.6e+5", s6);

	JString s7(1.57e5, 1, JString::kForceNoExponent);
	JAssertStringsEqual("157000.0", s7);

	JString s8(1.57e5, JString::kPrecisionAsNeeded, JString::kForceNoExponent, 0, 2);
	JAssertStringsEqual("160000", s8);

	JString s9(1, JString::kPrecisionAsNeeded, JString::kUseGivenExponent, -3);
	JAssertStringsEqual("1000e-3", s9);

	JString s10(1, JString::kPrecisionAsNeeded, JString::kUseGivenExponent, +2);
	JAssertStringsEqual("0.01e+2", s10);

	JString s11(537.6, 2, JString::kForceExponent);
	JAssertStringsEqual("5.38e+2", s11);

	JString s12(5.374, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	JAssertStringsEqual("5.37", s12);

	JString s13(5.375, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	JAssertStringsEqual("5.38", s13);

	JString s14(5.376, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 3);
	JAssertStringsEqual("5.38", s14);

	JString s15(9.995, 2);
	JAssertStringsEqual("10.00", s15);

	JString s16("abc");
	JAssertFalse(s16.ConvertToFloat(&f));
	JAssertEqual(0.0, f);

	JString s17(0.002, 0);
	JAssertStringsEqual("0", s17);
}

JTEST(Concatenate)
{
	JString s("5", 0);

	s.Prepend("");
	s.Append("");
	JAssertStringsEqual("5", s);

	s.Prepend("4");
	JAssertStringsEqual("45", s);

	s.Append("6");
	JAssertStringsEqual("456", s);

	s.Prepend("30", 1);
	JAssertStringsEqual("3456", s);

	s.Append("70", 1);
	JAssertStringsEqual("34567", s);

	s.Prepend(JUtf8Character("\xC3\xA6"));
	JAssertStringsEqual("\xC3\xA6" "34567", s);

	s.Append(JUtf8Character("\xCE\xA6"));
	JAssertStringsEqual("\xC3\xA6" "34567\xCE\xA6", s);

	s += "90\xE2\x9C\x94";
	JAssertStringsEqual("\xC3\xA6" "34567\xCE\xA6" "90\xE2\x9C\x94", s);

	s += JUtf8Character("\xCE\xA6");
	JAssertStringsEqual("\xC3\xA6" "34567\xCE\xA6" "90\xE2\x9C\x94\xCE\xA6", s);

	s += "\x6F\xCC\x88";	// test normalization
	JAssertStringsEqual("\xC3\xA6" "34567\xCE\xA6" "90\xE2\x9C\x94\xCE\xA6\xC3\xB6", s);
}

JTEST(Get)
{
	JString s("C", 1);
	JAssertEqual('C', s.GetFirstCharacter());
	JAssertEqual('C', s.GetLastCharacter());

	s = "123";
	JAssertEqual('1', s.GetFirstCharacter());
	JAssertEqual('3', s.GetLastCharacter());

	s = "\xC3\xA6" "34567\xCE\xA6" "90\xCE\xA6\xE2\x9C\x94";
	JAssertEqual(JUtf8Character("\xC3\xA6"), s.GetFirstCharacter());
	JAssertEqual(JUtf8Character("\xE2\x9C\x94"), s.GetLastCharacter());

	JUtf8Byte* s1 = s.AllocateBytes();
	JAssertStringsEqual(s, s1);
	delete [] s1;
	s1 = nullptr;

	JUtf8Character::SetIgnoreBadUtf8(true);
	s = "\xC3\xA6" "34567\xCE\xA6" "90\xCE\xA6\xF8\x9C\x94";
	JUtf8Character::SetIgnoreBadUtf8(false);
	JAssertEqual(JUtf8Character("\xC3\xA6"), s.GetFirstCharacter());
	JAssertEqual(JUtf8Character::kUtf8SubstitutionCharacter, s.GetLastCharacter());
}

JTEST(Search)
{
	TestString s;

	JIndex byteIndex;
	JAssertFalse(s.TestSearchForward("", 0, JString::kCompareCase, &byteIndex));
/*
	s.Set("beic");

	byteIndex = 1;
	JAssertTrue(s.TestSearchForward("\xC3\xA9\xC3\xAD\xC3\xA7", 6, JString::kIgnoreCase, &byteIndex));

	byteIndex = 4;
	JAssertTrue(s.TestSearchBackward("\xC3\xA9\xC3\xAD\xC3\xA7", 6, JString::kIgnoreCase, &byteIndex));
*/
	s.Set("\xC3\xB6\xE2\x9C\x94");
	JAssertTrue(s.StartsWith(JString("\x6F\xCC\x88\xE2\x9C\x94", JString::kCopy)));
	JAssertTrue(s.Contains(JString("\x6F\xCC\x88\xE2\x9C\x94", JString::kCopy)));
	JAssertTrue(s.EndsWith(JString("\x6F\xCC\x88\xE2\x9C\x94", JString::kCopy)));
	JAssertTrue(s.StartsWith("\x6F\xCC\x88\xE2\x9C\x94", JString::kIgnoreCase));
	JAssertTrue(s.Contains("\x6F\xCC\x88\xE2\x9C\x94", JString::kIgnoreCase));
	JAssertTrue(s.EndsWith("\x6F\xCC\x88\xE2\x9C\x94", JString::kIgnoreCase));

	byteIndex = 4;
	JAssertTrue(s.TestSearchBackward("\x6F\xCC\x88\xE2\x9C\x94", 6, JString::kIgnoreCase, &byteIndex));
}

JTEST(Contains)
{
	JString s;
	JAssertFalse(s.StartsWith("1"));
	JAssertFalse(s.Contains("1"));
	JAssertFalse(s.EndsWith("1"));

	s = "abcd";

	JAssertTrue(s.StartsWith("ab"));
	JAssertTrue(s.StartsWith("AB", JString::kIgnoreCase));
	JAssertFalse(s.StartsWith("bc"));
	JAssertFalse(s.StartsWith("BC", JString::kIgnoreCase));

	JAssertTrue(s.Contains("ab"));
	JAssertTrue(s.Contains("AB", JString::kIgnoreCase));
	JAssertTrue(s.Contains("bc"));
	JAssertTrue(s.Contains("BC", JString::kIgnoreCase));
	JAssertTrue(s.Contains("cd"));
	JAssertTrue(s.Contains("CD", JString::kIgnoreCase));
	JAssertFalse(s.Contains("abcdefg"));
	JAssertFalse(s.Contains("123"));

	JAssertTrue(s.EndsWith("cd"));
	JAssertTrue(s.EndsWith("CD", JString::kIgnoreCase));
	JAssertFalse(s.EndsWith("bc"));
	JAssertFalse(s.EndsWith("BC", JString::kIgnoreCase));

	s = "\xC3\x86" "a34567\xCE\xA6" "90b\xE2\x9C\x94\xCE\xA6";

	JAssertTrue(s.StartsWith("\xC3\x86" "a34"));
	JAssertTrue(s.StartsWith("\xC3\xA6" "A34", JString::kIgnoreCase));

	JAssertTrue(s.Contains("\xCE\xA6" "90b"));
	JAssertTrue(s.Contains("\xCF\x86" "90B", JString::kIgnoreCase));

	JAssertTrue(s.EndsWith("b\xE2\x9C\x94\xCE\xA6"));
	JAssertTrue(s.EndsWith("B\xE2\x9C\x94\xCF\x86", JString::kIgnoreCase));
	JAssertFalse(s.EndsWith("abc"));

	s = "\xC3\xB6";
	JString s1("\x6F\xCC\x88", 0);	// force normalization
	JAssertTrue(s.StartsWith(s1));
	JAssertTrue(s.Contains(s1));
	JAssertTrue(s.EndsWith(s1));

	s = "\x6F\xCC\x88";
	JAssertTrue(s.StartsWith("\xC3\xB6"));
	JAssertTrue(s.Contains("\xC3\xB6"));
	JAssertTrue(s.EndsWith("\xC3\xB6"));
}

JTEST(Whitespace)
{
	JString s;
	s.TrimWhitespace();
	JAssertStringsEqual("", s);

	s.Set("\t  foo\n");
	s.TrimWhitespace();
	JAssertStringsEqual("foo", s);
}

JTEST(IsAscii)
{
	JString s;
	JAssertTrue(s.IsAscii());

	s = "AbcD";
	JAssertTrue(s.IsAscii());

	s = "\xC3\x86\xCE\xA6\xCE\xA3";
	JAssertFalse(s.IsAscii());
}

JTEST(ToLower)
{
	JString s;
	s.ToLower();
	JAssertStringsEqual("", s);

	s = "ABCD";
	s.ToLower();
	JAssertStringsEqual("abcd", s);

	s = "abcd";
	s.ToLower();
	JAssertStringsEqual("abcd", s);

	// ae phi sigma

	s = "\xC3\x86\xCE\xA6\xCE\xA3";
	s.ToLower();
	JAssertStringsEqual("\xC3\xA6\xCF\x86\xCF\x82", s);

	s = "\xC3\xA6\xCF\x86\xCF\x82";
	s.ToLower();
	JAssertStringsEqual("\xC3\xA6\xCF\x86\xCF\x82", s);

	s = "\xC3\x86 \xCE\xA6 \xCE\xA3";
	s.ToLower();
	JAssertStringsEqual("\xC3\xA6 \xCF\x86 \xCF\x83", s);

	// greek

	s = "\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3";
	s.ToLower();
	JAssertStringsEqual("\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82", s);

	s = "\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s.ToLower();
	JAssertStringsEqual("\xCE\xBC\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82", s);
}

JTEST(ToUpper)
{
	JString s;
	s.ToUpper();
	JAssertStringsEqual("", s);

	s = "abcd";
	s.ToUpper();
	JAssertStringsEqual("ABCD", s);

	s = "ABCD";
	s.ToUpper();
	JAssertStringsEqual("ABCD", s);

	// ae phi sigma double-s

	s = "\xC3\xA6\xCF\x86\xCF\x83" "me\xC3\x9F";
	s.ToUpper();
	JAssertStringsEqual("\xC3\x86\xCE\xA6\xCE\xA3" "MESS", s);

	s = "\xC3\x86\xCE\xA6\xCE\xA3" "MESS";
	s.ToUpper();
	JAssertStringsEqual("\xC3\x86\xCE\xA6\xCE\xA3" "MESS", s);

	// greek

	s = "\xCE\x9C\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s.ToUpper();
	JAssertStringsEqual("\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3", s);

	s = "\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3";
	s.ToUpper();
	JAssertStringsEqual("\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3", s);
}

JTEST(MatchCase)
{
	JString s1, s2;
	s1.MatchCase(s2, JCharacterRange());
	JAssertTrue(s1.IsEmpty());

	s1 = "t";
	s2 = "A";
	s1.MatchCase(s2, JCharacterRange(1,1));
	JAssertStringsEqual("T", s1);

	s1 = "B";
	s1.MatchCase(s2, JCharacterRange(1,1));
	JAssertStringsEqual("B", s1);

	s1 = "ab";
	s1.MatchCase(s2, JCharacterRange(1,1));
	JAssertStringsEqual("AB", s1);

	s1 = "ab";
	s2 = "xy";
	s1.MatchCase(s2, JCharacterRange(1,2));
	JAssertStringsEqual("ab", s1);

	s1 = "ab";
	s2 = "Xy";
	s1.MatchCase(s2, JCharacterRange(1,2));
	JAssertStringsEqual("Ab", s1);

	s1 = "ab";
	s2 = "xY";
	s1.MatchCase(s2, JCharacterRange(1,2));
	JAssertStringsEqual("aB", s1);

	s1 = "ab";
	s2 = "XY";
	s1.MatchCase(s2, JCharacterRange(1,2));
	JAssertStringsEqual("AB", s1);

	s1 = "ab";
	s2 = "XY";
	s1.MatchCase(s2, JCharacterRange(1,1), JCharacterRange(1,1));
	JAssertStringsEqual("Ab", s1);

	s1 = "test test test";
	s2 = "Abc";
	s1.MatchCase(s2, JCharacterRange(1,3), JCharacterRange(6,9));
	JAssertStringsEqual("test Test test", s1);

	s1 = "test test test";
	s2 = "aBc";
	s1.MatchCase(s2, JCharacterRange(1,3), JCharacterRange(6,9));
	JAssertStringsEqual("test test test", s1);

	s1 = "test test test";
	s2 = "aBDc";
	s1.MatchCase(s2, JCharacterRange(1,4), JCharacterRange(6,9));
	JAssertStringsEqual("test tESt test", s1);

	s1 = "ABCD";
	s2 = "test tEsT test";
	s1.MatchCase(s2, JCharacterRange(6,9), JCharacterRange(1,4));
	JAssertStringsEqual("aBcD", s1);

	// greek

	s1 = "\xCE\x9C\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s2 = "XXXXX";
	s1.MatchCase(s2, JCharacterRange(1,5));
	JAssertStringsEqual("\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3", s1);
}

JTEST(Split)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

	JString s("|foo|bar|baz|", JString::kNoCopy);
	JString separator("|", JString::kNoCopy);

	s.Split(separator, &list);
	JAssertEqual(4, list.GetItemCount());
	JAssertStringsEqual("", *list.GetItem(1));
	JAssertStringsEqual("foo", *list.GetItem(2));
	JAssertStringsEqual("bar", *list.GetItem(3));
	JAssertStringsEqual("baz", *list.GetItem(4));

	separator = ",";

	s = "1,2,x,z,";
	s.Split(separator, &list);
	JAssertEqual(4, list.GetItemCount());
	JAssertStringsEqual("1", *list.GetItem(1));
	JAssertStringsEqual("2", *list.GetItem(2));
	JAssertStringsEqual("x", *list.GetItem(3));
	JAssertStringsEqual("z", *list.GetItem(4));

	s = ",1,2,x,z";
	s.Split(separator, &list);
	JAssertEqual(5, list.GetItemCount());
	JAssertStringsEqual("", *list.GetItem(1));
	JAssertStringsEqual("1", *list.GetItem(2));
	JAssertStringsEqual("2", *list.GetItem(3));
	JAssertStringsEqual("x", *list.GetItem(4));
	JAssertStringsEqual("z", *list.GetItem(5));

	s = "1,2,x,z,,";
	s.Split(separator, &list);
	JAssertEqual(5, list.GetItemCount());
	JAssertStringsEqual("1", *list.GetItem(1));
	JAssertStringsEqual("2", *list.GetItem(2));
	JAssertStringsEqual("x", *list.GetItem(3));
	JAssertStringsEqual("z", *list.GetItem(4));
	JAssertStringsEqual("", *list.GetItem(5));
}

#include <iomanip>
#include <unicode/unorm2.h>

JTEST(MatchLength)
{
	JString s1, s2;
	JAssertEqual(0, JString::CalcCharacterMatchLength(s1, s2));

	s1 = "abc";
	s2 = "abd";
	JAssertEqual(2, JString::CalcCharacterMatchLength(s1, s2));

	s2 = "xyz";
	JAssertEqual(0, JString::CalcCharacterMatchLength(s1, s2));

	s2 = "ayz";
	JAssertEqual(1, JString::CalcCharacterMatchLength(s1, s2));

	s2 = "xbz";
	JAssertEqual(0, JString::CalcCharacterMatchLength(s1, s2));

	s2 = "aBd";
	JAssertEqual(1, JString::CalcCharacterMatchLength(s1, s2));

	s2 = "aBd";
	JAssertEqual(2, JString::CalcCharacterMatchLength(s1, s2, JString::kIgnoreCase));

	// greek

	s1 = "\xCE\x9C\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s2 = "\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3";
	JAssertEqual(1, JString::CalcCharacterMatchLength(s1, s2));

	s1 = "\xCE\x9C\xCE\xAC\xCF\x8A\xCE\xBF\xCF\x82";
	s2 = "\xCE\x9C\xCE\x86\xCE\xAA\xCE\x9F\xCE\xA3";
	JAssertEqual(5, JString::CalcCharacterMatchLength(s1, s2, JString::kIgnoreCase));

	// test normalization of o-umlaut variants

	s1 = "\xC3\xB6\xE2\x9C\x94";
	s2 = "\x6F\xCC\x88\xE2\x9C\x94";
	JAssertEqual(2, JString::CalcCharacterMatchLength(s1, s2));

	s1 = "\x6F\xCC\x88\xE2\x9C\x94";
	s2 = "\xC3\xB6\xE2\x9C\x94";
	JAssertEqual(2, JString::CalcCharacterMatchLength(s1, s2));

	s1 = "\xC3\xB6\xE2\x9C\x94";
	s2 = "\x6F\xCC\x88\xE2\x9C\x94";
	JAssertEqual(2, JString::CalcCharacterMatchLength(s1, s2, JString::kIgnoreCase));

	s1 = "\x6F\xCC\x88\xE2\x9C\x94";
	s2 = "\xC3\xB6\xE2\x9C\x94";
	JAssertEqual(2, JString::CalcCharacterMatchLength(s1, s2, JString::kIgnoreCase));

	s1 = "\x6F\xCC\x88\xE2\x9C\x94";
	s2 = "\x6F\xCC\x88\xE2\x9C\x94";
	JAssertEqual(2, JString::CalcCharacterMatchLength(s1, s2, JString::kIgnoreCase));

	s1 = "\x4F\xCC\x88\xE2\x9C\x94";
	s2 = "\xC3\xB6\xE2\x9C\x94";
	JAssertEqual(2, JString::CalcCharacterMatchLength(s1, s2, JString::kIgnoreCase));
}

JTEST(Base64)
{
	JString s;
	JString e = s.EncodeBase64();
	JString d;
	JAssertTrue(e.DecodeBase64(&d));
	JAssertStringsEqual(s, d);

	s = "\xC3\x86" "a34567\xCE\xA6" "90b\xE2\x9C\x94\xCE\xA6";
	e = s.EncodeBase64();
	JAssertTrue(e.DecodeBase64(&d));
	JAssertStringsEqual(s, d);
}

JTEST(PrintHex)
{
	JString s("!#");

	std::ostringstream str;
	s.PrintHex(str);

	JAssertStringsEqual("21 23 ", str.str().c_str());
}

JTEST(Compare)
{
	JAssertFalse(JString::Compare("", "1") == 0);
	JAssertFalse(JString::Compare("1", "12") == 0);

	JAssertEqual(-1, JString::Compare("Cmlinkb", "cmlinkb"));
	JAssertEqual( 0, JString::Compare("Cmlinkb", "cmlinkb", JString::kIgnoreCase));
	JAssertEqual(-1, JString::Compare("CMLINKB", "cmlinka"));
	JAssertEqual(+1, JString::Compare("CMLINKB", "cmlinka", JString::kIgnoreCase));

	JAssertEqual(-1, JString::Compare("CMLINK::B", "cmlink::a"));
	JAssertEqual(+1, JString::Compare("CMLINK::B", "cmlink::a", JString::kIgnoreCase));
	JAssertEqual(-1, JString::Compare("CMLink::KillProgram", "CMLink::kFrameChanged"));
	JAssertEqual(+1, JString::Compare("CMLink::kFrameChanged", "CMLink::KillProgram"));
	JAssertEqual(+1, JString::Compare("CMLink::KillProgram", "CMLink::kFrameChanged", JString::kIgnoreCase));
	JAssertEqual(-1, JString::Compare("CMLink::kFrameChanged", "CMLink::KillProgram", JString::kIgnoreCase));

	// h a-ring r vs haar - equivalent in Danish

	JAssertFalse(JString::Compare("h" "\xC3\xA5" "r", "haar") == 0);

	// both are o-umlaut

	JAssertEqual(0, JString::Compare(JString("\x6F\xCC\x88", JString::kCopy), "\xC3\xB6"));
	JAssertEqual(0, JString::Compare("\x6F\xCC\x88", "\xC3\xB6", JString::kIgnoreCase));
	JAssertEqual(0, JString::Compare("\x4F\xCC\x88", "\xC3\xB6", JString::kIgnoreCase));
	JAssertEqual(0, JString::CompareMaxNBytes("\xC3\xB6", "\x6F\xCC\x88", 3, JString::kIgnoreCase));

	JAssertTrue(JString("foo", false) == "foo");
	JAssertTrue("foo" == JString("foo", false));
}

JTEST(MemoryStreaming)
{
	JString s1("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);

	std::ostringstream out;
	out << s1;

	std::istringstream in(out.str());

	JString s2;
	in >> s2;

	JAssertEqual(s1, s2);
}

JTEST(FileStreaming)
{
	std::ifstream input("data/test_JString.txt");
	JAssertTrue(input.good());

	JString s;
	s.ReadDelimited(input);
	JAssertTrue(input.good());
	JAssertStringsEqual("This is \xC3\xA2 test", s);

	s.ReadDelimited(input);
	JAssertTrue(input.good());
	JAssertStringsEqual("\\This is a \\ test\\", s);

	s.ReadDelimited(input);
	JAssertTrue(input.good());
	JAssertStringsEqual("\\\"This is a \\ test\"\\", s);

	s.ReadDelimited(input);
	JAssertTrue(input.good());
	JAssertStringsEqual("Jane said, \"Hello!\" didn't she?", s);

	s.ReadDelimited(input);
	JAssertTrue(input.good());
	JAssertEqual(1100, s.GetCharacterCount());
}

JTEST(CopyNormalizedBytes)
{
	const JIndex kStringLength = 10;
	JUtf8Byte string [ kStringLength ];

	const JUtf8Byte* stringList[] = {"", "1", "12345678", "123456789",
									 "1234567890", "123456789abcdef0"};

	for (const JUtf8Byte* test : stringList)
	{
		const JSize srcLength = strlen(test);
		const bool allCopied =
			JString::CopyNormalizedBytes(test, srcLength, string, kStringLength) == srcLength;

		JAssertEqualWithMessage(!strcmp(string, test), allCopied, test);
		JAssertEqualWithMessage(0, JString::CompareMaxNBytes(string, test, kStringLength-1), test);
		JAssertEqualWithMessage(JMin(kStringLength-1, srcLength), strlen(string), string);
	}
}

JTEST(Normalization)
{
	JString s(JString::kDoNotNormalize);
	s.Set("a\0b", 3);
	JAssertEqual(3, s.GetByteCount());

	JString s1(JString::kDoNotNormalize);
	s1 = "\xC3\xB6\xE2\x9C\x94";
	JAssertStringsEqual("\xC3\xB6\xE2\x9C\x94", s1);

	JString s2(JString::kDoNotNormalize);
	s2 = "\x6F\xCC\x88\xE2\x9C\x94";
	JAssertStringsEqual("\x6F\xCC\x88\xE2\x9C\x94", s2);

	JString s3;
	s3 = "\x6F\xCC\x88\xE2\x9C\x94";
	JAssertStringsEqual("\xC3\xB6\xE2\x9C\x94", s3);
}
