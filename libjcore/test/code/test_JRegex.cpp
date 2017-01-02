/******************************************************************************
 test.cc

	A test program for JRegex.  Designed to test JRegex code, not PCRE.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.
	Copyright (C) 2016 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <JRegex.h>
#include <locale.h>
#include <jAssert.h>

int main()
{
	std::cout << "Current locale: " << setlocale(LC_ALL, "") << std::endl;
	return JUnitTestManager::Execute();
}

class JTestRegex : public JRegex
{
public:

	JTestRegex()
		:
		JRegex()
	{ };

	JTestRegex(const JUtf8Byte* pattern)
		:
		JRegex(pattern)
	{ };

	JStringMatch
	MatchForward(const JString& str, const JIndex byteIndex) const
	{
		return JRegex::MatchForward(str, byteIndex);
	};

	JStringMatch
	MatchBackward(const JString& str, const JIndex byteIndex) const
	{
		return JRegex::MatchBackward(str, byteIndex);
	};
};

/******************************************************************************
 ShouldMatch

 *****************************************************************************/

void
ShouldMatch
	(
	JTestRegex&      regex,
	const JUtf8Byte* string,
	const JUtf8Byte* expectedMatch,
	const JIndex     stringByteIndex = 1
	)
{
	JString s(string, 0, kJFalse);
	const JStringMatch m = regex.MatchForward(s, stringByteIndex);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual(expectedMatch, m.GetString());
}

/******************************************************************************
 ShouldNotMatch

 *****************************************************************************/

void
ShouldNotMatch
	(
	JTestRegex&      regex,
	const JUtf8Byte* string,
	const JIndex     stringByteIndex = 1
	)
{
	JString s(string, 0, kJFalse);
	const JStringMatch m = regex.MatchForward(s, stringByteIndex);
	JAssertTrueWithMessage(m.IsEmpty(), string);
}

/******************************************************************************
 Tests

 *****************************************************************************/

JTEST(Assignment)
{
	JRegex *regexList[2];
	JRegex regex("c");
	JRegex regex2;

// Ensure that an immediate restore works
	regex.RestoreDefaults();
	regex2.RestoreDefaults();

// Test whether dialect works:
	JAssertOK(regex.SetPattern("a+x*b+"));
	JAssertEqual(0, regex.GetSubexpressionCount());

	regex2 = regex;

	regexList[0] = &regex;
	regexList[1] = &regex2;
	for (JIndex i=0; i<2; i++)
		{
		JAssertFalse(regexList[i]->Match(JString("qqqqqa+x*b+qqqqqq", 0, kJFalse)));
		JAssertFalse(regexList[i]->Match(JString("a+xxxxb+", 0, kJFalse)));
		JAssertTrue(regexList[i]->Match(JString("axxxb", 0, kJFalse)));
		JAssertFalse(regexList[i]->Match(JString("xxxxxxxxb", 0, kJFalse)));
		JAssertTrue(regexList[i]->Match(JString("aaabb", 0, kJFalse)));
		JAssertFalse(regexList[i]->Match(JString("aaaxxx", 0, kJFalse)));
		JAssertTrue(regexList[i]->Match(JString("aaaabbb", 0, kJFalse)));
		}
}

JTEST(Exercise)
{
// Test constructors
	JTestRegex regex;
	JAssertEqual(0, regex.GetSubexpressionCount());

// Test case-sensitivity
	JAssertOK(regex.SetPattern("aBc"));
	regex.RestoreDefaults();
	JAssertTrue(regex.IsCaseSensitive());
	ShouldMatch(regex, "qqqqqqqqqqaBcqqqqq", "aBc");
	ShouldNotMatch(regex, "qqqqqqqqqqabcqqqqq");

	regex.SetCaseSensitive(kJFalse);
	JAssertFalse(regex.IsCaseSensitive());
	ShouldMatch(regex, "qqqqqqqqqqaBcqqqqq", "aBc");
	ShouldMatch(regex, "qqqqqqqqqqabcqqqqq", "abc");

// Test single line
	JAssertOK(regex.SetPattern("^aBc"));
	regex.RestoreDefaults();
	JAssertFalse(regex.IsSingleLine());
	regex.SetSingleLine(kJTrue);
	JAssertTrue(regex.IsSingleLine());
	ShouldMatch(regex, "aBc", "aBc");
	ShouldNotMatch(regex, "\naBc");

	regex.SetSingleLine(kJFalse);
	JAssertFalse(regex.IsSingleLine());
	ShouldMatch(regex, "aBc", "aBc");
	ShouldMatch(regex, "\naBc", "aBc");

// Test line begin
	regex.RestoreDefaults();
	ShouldMatch(regex, "aBc", "aBc");

	regex.SetLineBegin(kJFalse);
	JAssertFalse(regex.IsLineBegin());
	ShouldNotMatch(regex, "aBc");

// Test line end
	JAssertOK(regex.SetPattern("aBc$"));
	regex.RestoreDefaults();
	JAssertTrue(regex.IsLineEnd());
	ShouldMatch(regex, "aBc", "aBc");

	regex.SetLineEnd(kJFalse);
	JAssertFalse(regex.IsLineEnd());
	ShouldNotMatch(regex, "aBc");

// Test single line again
	regex.SetSingleLine(kJFalse);
	JAssertFalse(regex.IsSingleLine());
	ShouldNotMatch(regex, "aBc");
	ShouldMatch(regex, "aBc\n", "aBc");

// Test error stuff
	regex.RestoreDefaults();
	JAssertFalse(regex.SetPattern("(ab").OK());
	JAssertFalse(regex.SetPattern("ab[").OK());

// Test MatchFrom
	JAssertOK(regex.SetPattern("x"));
	regex.RestoreDefaults();
	const JUtf8Byte* gString = "12345678xx1234568901";
	ShouldMatch(regex, gString, "x");
	ShouldMatch(regex, gString, "x", 8);
	ShouldMatch(regex, gString, "x", 9);
	ShouldMatch(regex, gString, "x", 10);
	ShouldNotMatch(regex, gString, 11);

// Test parenthesis capturing, replacement

	JString parenTest("-------------aJYYYb-----------", 0);

	JAssertOK(regex.SetPattern("a([IJ])([XY]+)b"));
	JAssertEqual(2, regex.GetSubexpressionCount());
	const JStringMatch m1 = regex.MatchForward(parenTest, 1);
	JAssertEqual(2, m1.GetSubstringCount());
	JAssertStringsEqual("aJYYYb", m1.GetString());
	JAssertStringsEqual("J", m1.GetSubstring(1));
	JAssertStringsEqual("YYY", m1.GetSubstring(2));

	// What if some subexpressions don't match?
	JAssertOK(regex.SetPattern("(a)(WontMatch)?(J)"));
	JAssertEqual(3, regex.GetSubexpressionCount());
	const JStringMatch m2 = regex.MatchForward(parenTest, 1);
	JAssertEqual(3, m2.GetSubstringCount());
	JAssertStringsEqual("aJ", m2.GetString());
	JAssertStringsEqual("a", m2.GetSubstring(1));
	JAssertTrue(m2.GetSubstring(2).IsEmpty());
	JAssertStringsEqual("J", m2.GetSubstring(3));

	parenTest = "bb";

	JAssertOK(regex.SetPattern("^(x*)|(a)"));
	JAssertEqual(2, regex.GetSubexpressionCount());
	const JStringMatch m3 = regex.MatchForward(parenTest, 1);
	JAssertEqual(1, m3.GetSubstringCount());
	JAssertTrue(m3.GetSubstring(1).IsEmpty());
	JAssertTrue(m3.GetSubstring(2).IsEmpty());

	// Test replacement
#if 0
	{
		const JString buffer = "        DRegex::Replace(      ";

		JString thisBuffer = buffer; // Don't modify buffer so we can reuse it.
		ShouldCompile(regex, "D([[:alnum:]_]+)::([[:alnum:]_]+)");

		JString replacePattern = "J$1::$002";
		Assert(regex.SetReplacePattern(replacePattern), "SetReplacePattern failed");
		Assert(regex.Match(thisBuffer, gMatchList), "Match failed");
		Assert(gMatchList->GetElement(1).Is(9, 23), "Located range incorrect");
		JString replacedString = regex.InterpolateMatches(thisBuffer, *gMatchList);
		Assert(replacedString == "JTestRegex::Replace",
			   "Interpolation failed, actual string is \"" + replacedString + "\"");

		Assert(!(regex.SetReplacePattern("$$1")).OK(), "SetReplacePattern succeeded incorrectly");

		replacePattern = "\\$a \\$- \\$ $00 $1 $+2 '$+3' '$9' \"$-1\" $-2 "
					   "$-3 '$-4' '$-10' \\$";
		Assert(regex.SetReplacePattern(replacePattern), "SetReplacePattern failed");
		replacedString = regex.InterpolateMatches(thisBuffer, *gMatchList);

		const JString expectedString = "$a $- $ DRegex::Replace Regex Replace '' '' \"Replace\" "
						 "Regex DRegex::Replace '' '' $";
		Assert(replacedString == expectedString,
			   "Interpolation failed; expected\n      \"" + expectedString
			   + "\n\" actual string was\n      \"" + replacedString + "\"");

		replacePattern = "J" "$+0001::" "$-1";
		Assert(regex.SetReplacePattern(replacePattern), "SetReplacePattern failed");
		// Test regular form of replace with replacement
		regex.SetLiteralReplace(kJFalse);
		JIndexRange newRange;
		regex.Replace(&thisBuffer, *gMatchList, &newRange);
		Assert(thisBuffer == "        JTestRegex::Replace(      ", "Replace failed");
		Assert(newRange == gMatchList->GetElement(1), "Range report incorrect");
		// Test regular form of replace without replacement
		thisBuffer = buffer;
		regex.SetLiteralReplace();
		regex.Replace(&thisBuffer, *gMatchList, &newRange);
		Assert(thisBuffer == "        J" "$+0001::" "$-1(      ", "Replace failed");
		// Test shorthand
		thisBuffer = buffer;
		regex.Replace(&thisBuffer, gMatchList->GetElement(1), &newRange);
		Assert(thisBuffer == "        J" "$+0001::" "$-1(      ", "Replace failed");


		// Test ajacent replacement

		thisBuffer = " XabX ";
		ShouldCompile(regex, "X(a)(b)X");
		replacePattern = "$2$3$0$-4$1";

		Assert(regex.SetReplacePattern(replacePattern), "SetReplacePattern failed");
		Assert(regex.Match(thisBuffer, gMatchList), "Match failed");

		Assert(gMatchList->GetElementCount() == 3, "Range count incorrect");
		Assert(gMatchList->GetElement(1).Is(2, 5), "Located range incorrect");
		Assert(gMatchList->GetElement(2).Is(3, 3), "Located range incorrect");
		Assert(gMatchList->GetElement(3).Is(4, 4), "Located range incorrect");

		regex.SetLiteralReplace(kJFalse);
		regex.Replace(&thisBuffer, *gMatchList, &newRange);
		Assert(newRange.Is(2, 7), "Range report incorrect");
		Assert(thisBuffer == " bXabXa ",
			   "Replaced string was \"" + thisBuffer + "\", should have been \" bXabXa \"");
	}

	// Test large replace indices
	{
		// Match 15 characters, one at a time
		ShouldCompile(regex, "(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)(.)");

		// Various replace patterns which reproduce the original
		JCharacter* patternList[] =
			{
			"$0",
			"$1$2$3$4$5$6$7$8$9$10$11$12$13$14$15$16",
			"$-17$-15$-14$-13$-12$-11$-10$-9$-8$-7$-6$-5$-4$-3$-2$-1",
			"$-16",
			NULL
			};

		const JCharacter* const buffer = "abcdefghijklmno"; // 15 characters
		JIndex i=0;
		while (patternList[i] != NULL)
			{
			JString thisBuffer = buffer;

			Assert(regex.SetReplacePattern(patternList[i]),
							 "SetReplacePattern failed");

			regex.Match(thisBuffer, gMatchList);
			Assert(gMatchList->GetElementCount() == 16);

			JIndexRange newRange;
			regex.Replace(&thisBuffer, *gMatchList, &newRange);
			Assert(newRange.GetLength() == 15, "Matched wrong range");
			Assert(thisBuffer == buffer,
				   "Replaced string is \"" + thisBuffer + "\" instead of \"" + buffer + "\"");

			++i;
			}
	}

	// Test case-matching replacement
	{
		// Match a word
		ShouldCompile(regex, "[a-zA-Z0-9]+");

		const JCharacter* buffer = "  abc  ";

		// Without case-coercion
		const JCharacter* replacePattern = "XYZ";
		Assert(regex.SetReplacePattern(replacePattern), "SetReplacePattern failed");

		// Better match
		Assert(regex.Match(buffer, gMatchList), "Match failed");
		Assert(gMatchList->GetElement(1).Is(3, 5), "Located range incorrect");

		// Replace without case matching
		JString replacedString = regex.InterpolateMatches(buffer, *gMatchList);

		Assert(replacedString == "XYZ",
			   "Interpolation failed, actual string is \"" + replacedString + "\"");

// Replace with case matching

		// Force all characters to change
		regex.SetMatchCase();
		replacedString = regex.InterpolateMatches(buffer, *gMatchList);

		Assert(replacedString == "xyz",
			   "Interpolation failed, actual string is \"" + replacedString + "\"");



		// Change first character only (not any more)
		buffer = "  ABc  ";

		// Better match
		Assert(regex.Match(buffer, gMatchList), "Match failed");
		Assert(gMatchList->GetElement(1).Is(3, 5), "Located range incorrect");

		replacePattern = "xyz";
		Assert(regex.SetReplacePattern(replacePattern), "SetReplacePattern failed");

		regex.SetMatchCase();
		replacedString = regex.InterpolateMatches(buffer, *gMatchList);

		Assert(replacedString == "XYz",
			   "Interpolation failed, actual string is \"" + replacedString + "\"");



		// Change all again, but in opposite directions
		buffer = "  aBC  ";

		// Better match
		Assert(regex.Match(buffer, gMatchList), "Match failed");
		Assert(gMatchList->GetElement(1).Is(3, 5), "Located range incorrect");

		replacePattern = "Xyz";
		Assert(regex.SetReplacePattern(replacePattern), "SetReplacePattern failed");

		regex.SetMatchCase();
		replacedString = regex.InterpolateMatches(buffer, *gMatchList);

		Assert(replacedString == "xYZ",
			   "Interpolation failed, actual string is \"" + replacedString + "\"");
	}
#endif
// Test MatchBackward

	JString backwardTest("a23456bc90123d567efg12hi", 0);

	JAssertOK(regex.SetPattern("[a-z]"));
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 1);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("a", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 3);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("a", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 6);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("a", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 7);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("b", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 8);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("c", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 9);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("c", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 10);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("c", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 13);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("c", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 19);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("f", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 20);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("g", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 21);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("g", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 23);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("h", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward(backwardTest, 24);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("i", m.GetString());
	}

// This sequence of calls caused a complicated bug at one time

	JTestRegex regex3;
	JAssertOK(regex3.SetPattern("a"));
	JAssertFalse(regex.SetPattern("*").OK());
	regex3.RestoreDefaults();

// Test compilation of a zero-length string

	JAssertFalse(regex.SetPattern("").OK());

// Test GetSubexpressionCount()'s recompilation

	regex.RestoreDefaults();
	JAssertOK(regex.SetPattern("a(b*)(c*)d"));
	JAssertEqual(2, regex.GetSubexpressionCount());

	JAssertOK(regex.SetPattern("a(b*)d"));
	JAssertEqual(1, regex.GetSubexpressionCount());


// Test JCore Extensions

	JString jcoreTest(" abc  def  ", 0);

	// Testing word boundaries
	regex.RestoreDefaults();
	JAssertOK(regex.SetPattern("\\b(.+)\\b"));

	const JStringMatch m5 = regex.MatchForward(jcoreTest, 1);
	JAssertStringsEqual("abc  def", m5.GetString());

	jcoreTest = "abc  def";
	const JStringMatch m6 = regex.MatchForward(jcoreTest, 1);
	JAssertStringsEqual("abc  def", m6.GetString());
}

JTEST(Features)
{
// Who can use backreferences?
	JTestRegex regex;
	JAssertOK(regex.SetPattern("([bc])\\1"));
	ShouldMatch(regex, "bb", "bb");
// These tests are not needed to show that ERE's don't have backref's, but can
// be added back in if Spencer ever fixes this.
	ShouldMatch(regex, "cc", "cc");
	ShouldNotMatch(regex, "bc");
	ShouldNotMatch(regex, "cb");
	ShouldNotMatch(regex, "b1");
	ShouldNotMatch(regex, "c1");

	JAssertOK(regex.SetPattern("(a+)b\\1"));
	ShouldMatch(regex, "aaabaaa", "aaabaaa");
	ShouldMatch(regex, "aaabaa", "aabaa");
	ShouldNotMatch(regex, "aaabbaa");
	ShouldNotMatch(regex, "aaabc");
	ShouldNotMatch(regex, "ab1");
	ShouldNotMatch(regex, "aaab1");
}
