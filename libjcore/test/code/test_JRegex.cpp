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
	MatchForward(const JUtf8Byte* str, const JIndex byteIndex = 1) const
	{
		itsMatchTarget.Set(str);
		return JRegex::MatchForward(itsMatchTarget, byteIndex);
	};

	JStringMatch
	MatchBackward(const JUtf8Byte* str, const JIndex byteIndex = 1) const
	{
		itsMatchTarget.Set(str);
		return JRegex::MatchBackward(itsMatchTarget, byteIndex);
	};

private:

	mutable JString	itsMatchTarget;
};

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
	{
		const JStringMatch m = regex.MatchForward("qqqqqqqqqqaBcqqqqq");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aBc", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("qqqqqqqqqqabcqqqqq");
		JAssertTrue(m.IsEmpty());
	}

	regex.SetCaseSensitive(kJFalse);
	JAssertFalse(regex.IsCaseSensitive());
	{
		const JStringMatch m = regex.MatchForward("qqqqqqqqqqaBcqqqqq");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aBc", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("qqqqqqqqqqabcqqqqq");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("abc", m.GetString());
	}

// Test single line
	JAssertOK(regex.SetPattern("^aBc"));
	regex.RestoreDefaults();
	JAssertFalse(regex.IsSingleLine());
	regex.SetSingleLine(kJTrue);
	JAssertTrue(regex.IsSingleLine());
	{
		const JStringMatch m = regex.MatchForward("aBc");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aBc", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("\naBc");
		JAssertTrue(m.IsEmpty());
	}

	regex.SetSingleLine(kJFalse);
	JAssertFalse(regex.IsSingleLine());
	{
		const JStringMatch m = regex.MatchForward("aBc");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aBc", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("\naBc");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aBc", m.GetString());
	}

// Test line begin
	regex.RestoreDefaults();
	{
		const JStringMatch m = regex.MatchForward("aBc");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aBc", m.GetString());
	}

	regex.SetLineBegin(kJFalse);
	JAssertFalse(regex.IsLineBegin());
	{
		const JStringMatch m = regex.MatchForward("aBc");
		JAssertTrue(m.IsEmpty());
	}

// Test line end
	JAssertOK(regex.SetPattern("aBc$"));
	regex.RestoreDefaults();
	JAssertTrue(regex.IsLineEnd());
	{
		const JStringMatch m = regex.MatchForward("aBc");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aBc", m.GetString());
	}

	regex.SetLineEnd(kJFalse);
	JAssertFalse(regex.IsLineEnd());
	{
		const JStringMatch m = regex.MatchForward("aBc");
		JAssertTrue(m.IsEmpty());
	}

// Test single line again
	regex.SetSingleLine(kJFalse);
	JAssertFalse(regex.IsSingleLine());
	{
		const JStringMatch m = regex.MatchForward("aBc");
		JAssertTrue(m.IsEmpty());
	}
	{
		const JStringMatch m = regex.MatchForward("aBc\n");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aBc", m.GetString());
	}

// Test error stuff
	regex.RestoreDefaults();
	JAssertFalse(regex.SetPattern("(ab").OK());
	JAssertFalse(regex.SetPattern("ab[").OK());

// Test MatchFrom
	JAssertOK(regex.SetPattern("x"));
	regex.RestoreDefaults();
	{
		const JStringMatch m = regex.MatchForward("12345678xx1234568901");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("x", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("12345678xx1234568901", 8);
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("x", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("12345678xx1234568901", 9);
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("x", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("12345678xx1234568901", 10);
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("x", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("12345678xx1234568901", 11);
		JAssertTrue(m.IsEmpty());
	}

// Test parenthesis capturing, replacement

	JAssertOK(regex.SetPattern("a([IJ])([XY]+)b"));
	JAssertEqual(2, regex.GetSubexpressionCount());
	const JStringMatch m1 = regex.MatchForward("-------------aJYYYb-----------");
	JAssertEqual(2, m1.GetSubstringCount());
	JAssertStringsEqual("aJYYYb", m1.GetString());
	JAssertStringsEqual("J", m1.GetSubstring(1));
	JAssertStringsEqual("YYY", m1.GetSubstring(2));

	// What if some subexpressions don't match?
	JAssertOK(regex.SetPattern("(a)(WontMatch)?(J)"));
	JAssertEqual(3, regex.GetSubexpressionCount());
	const JStringMatch m2 = regex.MatchForward("-------------aJYYYb-----------");
	JAssertEqual(3, m2.GetSubstringCount());
	JAssertStringsEqual("aJ", m2.GetString());
	JAssertStringsEqual("a", m2.GetSubstring(1));
	JAssertTrue(m2.GetSubstring(2).IsEmpty());
	JAssertStringsEqual("J", m2.GetSubstring(3));

	JAssertOK(regex.SetPattern("^(x*)|(a)"));
	JAssertEqual(2, regex.GetSubexpressionCount());
	const JStringMatch m3 = regex.MatchForward("bb");
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

	JAssertOK(regex.SetPattern("[a-z]"));
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi");
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("a", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 3);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("a", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 6);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("a", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 7);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("b", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 8);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("c", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 9);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("c", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 10);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("c", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 13);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("c", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 19);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("f", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 20);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("g", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 21);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("g", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 23);
	JAssertFalse(m.IsEmpty());
	JAssertStringsEqual("h", m.GetString());
	}
	{
	const JStringMatch m = regex.MatchBackward("a23456bc90123d567efg12hi", 24);
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

	// Testing word boundaries
	regex.RestoreDefaults();
	JAssertOK(regex.SetPattern("\\b(.+)\\b"));

	const JStringMatch m5 = regex.MatchForward(" abc  def  ");
	JAssertStringsEqual("abc  def", m5.GetString());

	const JStringMatch m6 = regex.MatchForward("abc  def");
	JAssertStringsEqual("abc  def", m6.GetString());
}

JTEST(Features)
{
// Who can use backreferences?
	JTestRegex regex;
	JAssertOK(regex.SetPattern("([bc])\\1"));
	{
		const JStringMatch m = regex.MatchForward("bb");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("bb", m.GetString());
	}
// These tests are not needed to show that ERE's don't have backref's, but can
// be added back in if Spencer ever fixes this.
	{
		const JStringMatch m = regex.MatchForward("cc");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("cc", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("bc");
		JAssertTrue(m.IsEmpty());
	}
	{
		const JStringMatch m = regex.MatchForward("cb");
		JAssertTrue(m.IsEmpty());
	}
	{
		const JStringMatch m = regex.MatchForward("b1");
		JAssertTrue(m.IsEmpty());
	}
	{
		const JStringMatch m = regex.MatchForward("c1");
		JAssertTrue(m.IsEmpty());
	}

	JAssertOK(regex.SetPattern("(a+)b\\1"));
	{
		const JStringMatch m = regex.MatchForward("aaabaaa");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aaabaaa", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("aaabaa");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("aabaa", m.GetString());
	}
	{
		const JStringMatch m = regex.MatchForward("aaabbaa");
		JAssertTrue(m.IsEmpty());
	}
	{
		const JStringMatch m = regex.MatchForward("aaabc");
		JAssertTrue(m.IsEmpty());
	}
	{
		const JStringMatch m = regex.MatchForward("ab1");
		JAssertTrue(m.IsEmpty());
	}
	{
		const JStringMatch m = regex.MatchForward("aaab1");
		JAssertTrue(m.IsEmpty());
	}
}
