/******************************************************************************
 test_JRegex.cc

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

JTEST(Options)
{
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

// This sequence of calls caused a complicated bug at one time

	JTestRegex regex3;
	JAssertOK(regex3.SetPattern("a"));
	JAssertFalse(regex.SetPattern("*").OK());
	regex3.RestoreDefaults();

// Test compilation of a zero-length string

	JAssertFalse(regex.SetPattern("").OK());

// Test JCore Extensions

	// Testing word boundaries
	regex.RestoreDefaults();
	JAssertOK(regex.SetPattern("\\b(.+)\\b"));

	const JStringMatch m5 = regex.MatchForward(" abc  def  ");
	JAssertStringsEqual("abc  def", m5.GetString());

	const JStringMatch m6 = regex.MatchForward("abc  def");
	JAssertStringsEqual("abc  def", m6.GetString());
}

JTEST(MatchFrom)
{
	JTestRegex regex("x");
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
}

JTEST(Subexpressions)
{
	JTestRegex regex;

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

// Test UTF-8 & named subexpressions

	JAssertOK(regex.SetPattern("\xC3\x85(?P<foo>.)"));
	{
		const JStringMatch m = regex.MatchForward("1234567890\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
		JAssertFalse(m.IsEmpty());
		JAssertStringsEqual("\xC3\x85\xC3\xA5", m.GetString());
		JAssertEqual(1, m.GetSubstringCount());
		JAssertStringsEqual("\xC3\xA5", m.GetSubstring(1));
		JAssertStringsEqual("\xC3\xA5", m.GetSubstring("foo"));
	}

// Test GetSubexpressionCount()'s recompilation

	regex.RestoreDefaults();
	JAssertOK(regex.SetPattern("a(b*)(c*)d"));
	JAssertEqual(2, regex.GetSubexpressionCount());

	JAssertOK(regex.SetPattern("a(b*)d"));
	JAssertEqual(1, regex.GetSubexpressionCount());
}

JTEST(MatchBackward)
{
	JTestRegex regex("[a-z]");
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

JTEST(Split)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

	JString s("  foo bar  baz ", 0, kJFalse);

	JRegex regex("\\s+");
	regex.Split(s, &list);
	JAssertEqual(4, list.GetElementCount());
	JAssertStringsEqual("", *list.NthElement(1));
	JAssertStringsEqual("foo", *list.NthElement(2));
	JAssertStringsEqual("bar", *list.NthElement(3));
	JAssertStringsEqual("baz", *list.NthElement(4));

	regex.Split(s, &list, 0, kJTrue);
	JAssertEqual(8, list.GetElementCount());
	JAssertStringsEqual("", *list.NthElement(1));
	JAssertStringsEqual("  ", *list.NthElement(2));
	JAssertStringsEqual("foo", *list.NthElement(3));
	JAssertStringsEqual(" ", *list.NthElement(4));
	JAssertStringsEqual("bar", *list.NthElement(5));
	JAssertStringsEqual("  ", *list.NthElement(6));
	JAssertStringsEqual("baz", *list.NthElement(7));
	JAssertStringsEqual(" ", *list.NthElement(8));

	s.TrimWhitespace();

	regex.Split(s, &list, 2);
	JAssertEqual(2, list.GetElementCount());
	JAssertStringsEqual("foo", *list.NthElement(1));
	JAssertStringsEqual("bar  baz", *list.NthElement(2));

	s = "1,2,x,z,";
	regex.SetPatternOrDie(",");

	regex.Split(s, &list);
	JAssertEqual(4, list.GetElementCount());
	JAssertStringsEqual("1", *list.NthElement(1));
	JAssertStringsEqual("2", *list.NthElement(2));
	JAssertStringsEqual("x", *list.NthElement(3));
	JAssertStringsEqual("z", *list.NthElement(4));
}

JTEST(BackslashForLiteral)
{
	JAssertTrue(JRegex::NeedsBackslashToBeLiteral('$'));
	JAssertTrue(JRegex::NeedsBackslashToBeLiteral(JUtf8Character('$')));
	JAssertFalse(JRegex::NeedsBackslashToBeLiteral(JUtf8Character("\xE2\x9C\x94")));

	const JString s = JRegex::BackslashForLiteral(
		JString(".[foo]\\?*^${\xE2\x9C\x94}|()83", 0, kJFalse));
	JAssertStringsEqual("\\.\\[foo\\]\\\\\\?\\*\\^\\$\\{\xE2\x9C\x94\\}\\|\\(\\)83", s);
}
