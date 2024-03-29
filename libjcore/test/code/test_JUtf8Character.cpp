/******************************************************************************
 test_JUtf8Character.cpp

	Program to test JUtf8Character class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JUtf8Character.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Construction)
{
	JUtf8Character c1;
	JAssertEqual(0, c1.GetByteCount());

	JUtf8Character c2('1');
	JAssertEqual(1, c2.GetByteCount());

	JUtf8Character c3("1");
	JAssertEqual(1, c3.GetByteCount());

	JUtf8Character c4("\xC2\xA9");
	JAssertEqual(2, c4.GetByteCount());

	JUtf8Character c5("\xE2\x9C\x94");
	JAssertEqual(3, c5.GetByteCount());
	JAssertEqual(3, strlen(c5.GetBytes()));	// test null termination

	JUtf8Character c6(c5);
	JAssertEqual(3, c6.GetByteCount());

	JUtf8Character c7("abcd");
	JAssertEqual(1, c7.GetByteCount());

	JUtf8Character c8("\xE2\x9C\x94" "abcd");
	JAssertEqual(3, c8.GetByteCount());
}

JTEST(Set)
{
	JUtf8Character c;

	c.Set('1');
	JAssertEqual(1, c.GetByteCount());

	c.Set("1");
	JAssertEqual(1, c.GetByteCount());

	c.Set("\xC2\xA9");
	JAssertEqual(2, c.GetByteCount());

	c.Set("\xE2\x9C\x94");
	JAssertEqual(3, c.GetByteCount());

	JUtf8Character::SetIgnoreBadUtf8(true);
	c.Set("\xF4\x30");
	JUtf8Character::SetIgnoreBadUtf8(false);
	JAssertEqual(JUtf8Character::kUtf32SubstitutionCharacter, c.GetUtf32());

	JUtf8Character c2;
	c2.Set(c);
	JAssertEqual(3, c2.GetByteCount());

	JUtf8Character c3;
	c3 = c;
	JAssertEqual(3, c2.GetByteCount());

	JUtf8Character c4;
	c4 = '5';
	JAssertEqual(1, c4.GetByteCount());

	JUtf8Character c5;
	c5 = "\xE2\x9C\x94";
	JAssertEqual(3, c5.GetByteCount());
}

JTEST(IsValid)
{
	JAssertTrue(JUtf8Character::IsValid(""));
	JAssertTrue(JUtf8Character::IsValid("1"));
	JAssertTrue(JUtf8Character::IsValid("\xC2\xA9"));
	JAssertTrue(JUtf8Character::IsValid("\xE2\x9C\x94"));

	JUtf8Character::SetIgnoreBadUtf8(true);
	JAssertFalse(JUtf8Character::IsValid("\xC2\x0A"));
	JAssertFalse(JUtf8Character::IsValid("\xF6\x9C\x94"));
	JUtf8Character::SetIgnoreBadUtf8(false);
}

JTEST(IsBlank)
{
	JAssertTrue(JUtf8Character("").IsBlank());
	JAssertFalse(JUtf8Character("1").IsBlank());
	JAssertFalse(JUtf8Character("\xC2\xA9").IsBlank());
	JAssertFalse(JUtf8Character("\xE2\x9C\x94").IsBlank());
}

JTEST(IsAscii)
{
	JAssertTrue(JUtf8Character("").IsAscii());
	JAssertTrue(JUtf8Character("1").IsAscii());
	JAssertFalse(JUtf8Character("\xC2\xA9").IsAscii());
	JAssertFalse(JUtf8Character("\xE2\x9C\x94").IsAscii());
}

JTEST(IsCompleteCharacter)
{
	JSize byteCount;
	JAssertFalse(JUtf8Character::IsCompleteCharacter("", 0, &byteCount));
	JAssertEqual(0, byteCount);

	JAssertFalse(JUtf8Character::IsCompleteCharacter("5", 0, &byteCount));
	JAssertEqual(1, byteCount);

	JAssertTrue(JUtf8Character::IsCompleteCharacter("5", 1, &byteCount));
	JAssertEqual(1, byteCount);

	JAssertFalse(JUtf8Character::IsCompleteCharacter("\xC2", 1, &byteCount));
	JAssertEqual(2, byteCount);

	JAssertTrue(JUtf8Character::IsCompleteCharacter("\xC2\xA9", 2, &byteCount));
	JAssertEqual(2, byteCount);

	JAssertFalse(JUtf8Character::IsCompleteCharacter("\xE2", 1, &byteCount));
	JAssertEqual(3, byteCount);

	JAssertFalse(JUtf8Character::IsCompleteCharacter("\xE2\x9C", 2, &byteCount));
	JAssertEqual(3, byteCount);

	JAssertTrue(JUtf8Character::IsCompleteCharacter("\xE2\x9C\x94", 3, &byteCount));
	JAssertEqual(3, byteCount);

	JAssertTrue(JUtf8Character::IsCompleteCharacter("\xF0\xA0\x9D\xB9", 4, &byteCount));
	JAssertEqual(4, byteCount);
}

JTEST(CharacterByteCount)
{
	JSize byteCount;
	JAssertTrue(JUtf8Character::GetCharacterByteCount("", &byteCount));
	JAssertEqual(1, byteCount);

	JAssertTrue(JUtf8Character::GetCharacterByteCount("1", &byteCount));
	JAssertEqual(1, byteCount);

	JAssertTrue(JUtf8Character::GetCharacterByteCount("\xC2\xA9", &byteCount));
	JAssertEqual(2, byteCount);

	JAssertTrue(JUtf8Character::GetCharacterByteCount("\xE2\x9C\x94", &byteCount));
	JAssertEqual(3, byteCount);

	JUtf8Character::SetIgnoreBadUtf8(true);
	JAssertFalse(JUtf8Character::GetCharacterByteCount("\xF5\x9C\x94", &byteCount));
	JAssertFalse(JUtf8Character::GetCharacterByteCount("\xF4\x30\x94", &byteCount));
	JUtf8Character::SetIgnoreBadUtf8(false);
}

JTEST(PrevCharacter)
{
	const JUtf8Byte* s = "ABC";
	JSize byteCount;
	JAssertTrue(JUtf8Character::GetPrevCharacterByteCount(s + strlen(s) - 1, &byteCount));
	JAssertEqual(1, byteCount);

	s = "ABC\xF0\xAF\xA7\x97";
	JAssertTrue(JUtf8Character::GetPrevCharacterByteCount(s + strlen(s) - 1, &byteCount));
	JAssertEqual(4, byteCount);

	s = "AB\xF0\xAF\xA7\x97" "c";
	JAssertTrue(JUtf8Character::GetPrevCharacterByteCount(s + strlen(s) - 1, &byteCount));
	JAssertEqual(1, byteCount);

	s = "ABC\xF8\xAF\xA7\x97";
	JUtf8Character::SetIgnoreBadUtf8(true);
	JAssertFalse(JUtf8Character::GetPrevCharacterByteCount(s + strlen(s) - 1, &byteCount));
	JUtf8Character::SetIgnoreBadUtf8(false);
	JAssertEqual(1, byteCount);
}

JTEST(Comparison)
{
	JAssertTrue(JUtf8Character("") == '\0');
	JAssertTrue(JUtf8Character("1") == '1');
	JAssertTrue(JUtf8Character("1") != '2');
	JAssertTrue('1' == JUtf8Character("1"));
	JAssertTrue(JUtf8Character("\xD2\x82") == "\xD2\x82");
	JAssertTrue(JUtf8Character("\xD2\x82") != "\xD2\x84");
}

JTEST(Utf32ToUtf8)
{
	JAssertEqual(JUtf8Character(""), JUtf8Character::Utf32ToUtf8(0));
	JAssertEqual(JUtf8Character("1"), JUtf8Character::Utf32ToUtf8('1'));
	JAssertEqual(JUtf8Character("\xD2\x82"), JUtf8Character::Utf32ToUtf8(1154));
	JAssertEqual(JUtf8Character("\xE3\x80\xA8"), JUtf8Character::Utf32ToUtf8(12328));
	JAssertEqual(JUtf8Character("\xF0\xAF\xA7\x97"), JUtf8Character::Utf32ToUtf8(195031));
}

JTEST(Utf8ToUtf32)
{
	JAssertEqual(0, JUtf8Character("").GetUtf32());
	JAssertEqual('1', JUtf8Character("1").GetUtf32());
	JAssertEqual(1154, JUtf8Character("\xD2\x82").GetUtf32());
	JAssertEqual(12328, JUtf8Character("\xE3\x80\xA8").GetUtf32());
	JAssertEqual(195031, JUtf8Character("\xF0\xAF\xA7\x97").GetUtf32());

	JUtf8Character::SetIgnoreBadUtf8(true);
	JAssertEqual(65533, JUtf8Character("\xFE\xAF\xFF\x97").GetUtf32());
	JUtf8Character::SetIgnoreBadUtf8(false);
}

JTEST(IsPrint)
{
	JAssertFalse(JUtf8Character('\x1B').IsPrint());
	JAssertTrue(JUtf8Character('X').IsPrint());

	JAssertFalse(JUtf8Character("\xE2\x80\x8E").IsPrint());
	JAssertTrue(JUtf8Character("\xF0\xAF\xA7\x97").IsPrint());
}

JTEST(IsAlnum)
{
	JAssertFalse(JUtf8Character('{').IsAlnum());
	JAssertTrue(JUtf8Character('5').IsAlnum());
	JAssertTrue(JUtf8Character('X').IsAlnum());
	JAssertTrue(JUtf8Character("\xC3\xA6").IsAlnum());
	JAssertTrue(JUtf8Character("\xF0\xAF\xA7\x97").IsAlnum());
}

JTEST(IsDigit)
{
	JAssertFalse(JUtf8Character('{').IsDigit());
	JAssertTrue(JUtf8Character('5').IsDigit());
}

JTEST(IsAlpha)
{
	JAssertFalse(JUtf8Character('{').IsAlpha());
	JAssertFalse(JUtf8Character('-').IsAlpha());
	JAssertFalse(JUtf8Character('_').IsAlpha());
	JAssertFalse(JUtf8Character('5').IsAlpha());
	JAssertTrue(JUtf8Character('X').IsAlpha());
	JAssertTrue(JUtf8Character("\xC3\xA6").IsAlpha());
	JAssertTrue(JUtf8Character("\xCF\x86").IsAlpha());
	JAssertTrue(JUtf8Character("\xF0\xAF\xA7\x97").IsAlpha());
}

JTEST(IsLower)
{
	JAssertFalse(JUtf8Character('5').IsLower());

	JAssertTrue(JUtf8Character('b').IsLower());
	JAssertFalse(JUtf8Character('B').IsLower());

	JAssertTrue(JUtf8Character("\xC3\xA6").IsLower());	// ae
	JAssertFalse(JUtf8Character("\xC3\x86").IsLower());

	JAssertTrue(JUtf8Character("\xCF\x86").IsLower());	// phi
	JAssertFalse(JUtf8Character("\xCE\xA6").IsLower());

	JAssertTrue(JUtf8Character("\xCF\x83").IsLower());	// sigma
	JAssertFalse(JUtf8Character("\xCE\xA3").IsLower());
}

JTEST(IsUpper)
{
	JAssertFalse(JUtf8Character('5').IsUpper());

	JAssertFalse(JUtf8Character('b').IsUpper());
	JAssertTrue(JUtf8Character('B').IsUpper());

	JAssertFalse(JUtf8Character("\xC3\xA6").IsUpper());	// ae
	JAssertTrue(JUtf8Character("\xC3\x86").IsUpper());

	JAssertFalse(JUtf8Character("\xCF\x86").IsUpper());	// phi
	JAssertTrue(JUtf8Character("\xCE\xA6").IsUpper());

	JAssertFalse(JUtf8Character("\xCF\x83").IsUpper());	// sigma
	JAssertTrue(JUtf8Character("\xCE\xA3").IsUpper());
}

JTEST(IsSpace)
{
	JAssertTrue(JUtf8Character(' ').IsSpace());
	JAssertTrue(JUtf8Character('\n').IsSpace());
	JAssertFalse(JUtf8Character('5').IsSpace());
}

JTEST(IsControl)
{
	JAssertTrue(JUtf8Character('\b').IsControl());
	JAssertFalse(JUtf8Character(' ').IsControl());
	JAssertFalse(JUtf8Character('5').IsControl());
}

JTEST(ToLower)
{
	JAssertEqual(JUtf8Character("b"), JUtf8Character("b").ToLower());
	JAssertEqual(JUtf8Character("b"), JUtf8Character("B").ToLower());
	JAssertEqual(JUtf8Character("\xC3\xA6"), JUtf8Character("\xC3\xA6").ToLower());	// ae
	JAssertEqual(JUtf8Character("\xC3\xA6"), JUtf8Character("\xC3\x86").ToLower());	// ae
	JAssertEqual(JUtf8Character("\xC3\xA9"), JUtf8Character("\xC3\xA9").ToLower());	// 'e
	JAssertEqual(JUtf8Character("\xC3\xA9"), JUtf8Character("\xC3\x89").ToLower());	// 'e
	JAssertEqual(JUtf8Character("\xCF\x86"), JUtf8Character("\xCE\xA6").ToLower());	// phi
	JAssertEqual(JUtf8Character("\xCF\x83"), JUtf8Character("\xCE\xA3").ToLower());	// sigma
}

JTEST(ToUpper)
{
	JAssertEqual(JUtf8Character("B"), JUtf8Character("B").ToUpper());
	JAssertEqual(JUtf8Character("B"), JUtf8Character("b").ToUpper());
	JAssertEqual(JUtf8Character("\xC3\x86"), JUtf8Character("\xC3\x86").ToUpper());	// ae
	JAssertEqual(JUtf8Character("\xC3\x86"), JUtf8Character("\xC3\xA6").ToUpper());	// ae
	JAssertEqual(JUtf8Character("\xC3\x89"), JUtf8Character("\xC3\x89").ToUpper());	// 'e
	JAssertEqual(JUtf8Character("\xC3\x89"), JUtf8Character("\xC3\xA9").ToUpper());	// 'e
	JAssertEqual(JUtf8Character("\xCE\xA6"), JUtf8Character("\xCF\x86").ToUpper());	// phi
	JAssertEqual(JUtf8Character("\xCE\xA3"), JUtf8Character("\xCF\x83").ToUpper());	// sigma
}

JTEST(PrintHex)
{
	JUtf8Character c("\xC3\x86");

	std::ostringstream output;
	c.PrintHex(output);
	JAssertStringsEqual("c3 86 ", output.str().c_str());
}

JTEST(AllocateNullTerminatedBytes)
{
	JUtf8Byte* s = JUtf8Character('{').AllocateBytes();
	JAssertStringsEqual("{", s);
	jdelete [] s;

	s = JUtf8Character("\xC3\xA6").AllocateBytes();
	JAssertStringsEqual("\xC3\xA6", s);
	jdelete [] s;

	s = JUtf8Character("\xF0\xAF\xA7\x97").AllocateBytes();
	JAssertStringsEqual("\xF0\xAF\xA7\x97", s);
	jdelete [] s;
}

JTEST(Stream)
{
	std::ostringstream output;
	output << JUtf8Character()
		   << JUtf8Character("1")
		   << JUtf8Character("\xC2\xA9")
		   << JUtf8Character("\xE2\x9C\x94")
		   << JUtf8Character("\xF0\xAF\xA7\x97");
	output.write("\xC2\x0A", 2);

	std::istringstream input(output.str());

	JUtf8Character c;
	input >> c;
	JAssertStringsEqual("1", c.GetBytes());

	input >> c;
	JAssertStringsEqual("\xC2\xA9", c.GetBytes());

	input >> c;
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());

	input >> c;
	JAssertStringsEqual("\xF0\xAF\xA7\x97", c.GetBytes());

	JUtf8Character::SetIgnoreBadUtf8(true);
	input >> c;
	JUtf8Character::SetIgnoreBadUtf8(false);
	JAssertStringsEqual("\xEF\xBF\xBD", c.GetBytes());
}
