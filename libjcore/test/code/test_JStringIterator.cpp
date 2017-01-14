/******************************************************************************
 test_JStringIterator.cc

	Program to test JStringIterator class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <JRegex.h>
#include <locale.h>
#include <jAssert.h>

int main()
{
	std::cout << "Current locale: " << setlocale(LC_ALL, "") << std::endl;
	return JUnitTestManager::Execute();
}

JTEST(Iteration)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(s);
	JUtf8Character c;
	JIndex j;

	JAssertTrue(i.IsValid());
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertFalse(i.GetPrevCharacterIndex(&j));
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(1, j);

	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(1, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(2, j);
	JAssertEqual(1, i.GetPrevCharacterIndex());
	JAssertEqual(2, i.GetNextCharacterIndex());

	JAssertTrue(i.Next(&c));
	JAssertEqual('2', c);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(2, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(3, j);

	JAssertTrue(i.Next(&c));
	JAssertEqual('3', c);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(3, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(4, j);

	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC2\xA9", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(4, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(5, j);

	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\x85", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(5, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(6, j);

	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\xA5", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(7, j);

	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(7, j);
	JAssertFalse(i.GetNextCharacterIndex(&j));

	JAssertFalse(i.Next(&c));
	JAssertFalse(i.AtBeginning());
	JAssertTrue(i.AtEnd());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(7, j);
	JAssertFalse(i.GetNextCharacterIndex(&j));


	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(7, j);

	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(5, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(6, j);

	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\x85", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(4, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(5, j);

	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC2\xA9", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(3, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(4, j);

	JAssertTrue(i.Prev(&c));
	JAssertEqual('3', c);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(2, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(3, j);

	JAssertTrue(i.Prev(&c));
	JAssertEqual('2', c);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(1, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(2, j);

	JAssertTrue(i.Prev(&c));
	JAssertEqual('1', c);
	JAssertFalse(i.GetPrevCharacterIndex(&j));
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(1, j);

	JAssertFalse(i.Prev(&c));
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertFalse(i.GetPrevCharacterIndex(&j));
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(1, j);


	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(1, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(2, j);

	JAssertTrue(i.Prev(&c));
	JAssertEqual('1', c);
	JAssertFalse(i.GetPrevCharacterIndex(&j));
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(1, j);
}

JTEST(Move)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(s);
	JUtf8Character c;
	JIndex j;

	i.MoveTo(kJIteratorStartAtEnd, 0);
	JAssertFalse(i.AtBeginning());
	JAssertTrue(i.AtEnd());
	JAssertFalse(i.Next(&c));
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(7, j);
	JAssertFalse(i.GetNextCharacterIndex(&j));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(7, j);

	i.MoveTo(kJIteratorStartAtBeginning, 0);
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertFalse(i.Prev(&c));
	JAssertFalse(i.GetPrevCharacterIndex(&j));
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(1, j);
	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(1, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(2, j);

	i.MoveTo(kJIteratorStartBefore, 5);
	JAssertFalse(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(4, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(5, j);
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\x85", c.GetBytes());

	i.MoveTo(kJIteratorStartBefore, 1);
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertFalse(i.GetPrevCharacterIndex(&j));
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(1, j);
	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);

	i.MoveTo(kJIteratorStartBeforeByte, 6);
	JAssertFalse(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(4, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(5, j);
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\x85", c.GetBytes());

	i.MoveTo(kJIteratorStartBeforeByte, 1);
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertFalse(i.GetPrevCharacterIndex(&j));
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(1, j);
	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);

	i.MoveTo(kJIteratorStartAfter, 6);
	JAssertFalse(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(7, j);
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", c.GetBytes());

	i.MoveTo(kJIteratorStartAfter, 7);
	JAssertFalse(i.AtBeginning());
	JAssertTrue(i.AtEnd());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(7, j);
	JAssertFalse(i.GetNextCharacterIndex(&j));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());

	i.MoveTo(kJIteratorStartAfterByte, 9);
	JAssertFalse(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(7, j);
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", c.GetBytes());

	i.MoveTo(kJIteratorStartAfterByte, s.GetByteCount());
	JAssertFalse(i.AtBeginning());
	JAssertTrue(i.AtEnd());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(7, j);
	JAssertFalse(i.GetNextCharacterIndex(&j));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
}

JTEST(Skip)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(s);
	JUtf8Character c;
	JIndex j;

	i.MoveTo(kJIteratorStartAtBeginning, 0);
	i.SkipNext(5);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(5, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\xA5", c.GetBytes());

	i.MoveTo(kJIteratorStartAtEnd, 0);
	i.SkipPrev(2);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(5, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\x85", c.GetBytes());
}

JTEST(Search)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(s);
	JUtf8Character c;
	JIndex j;

	JAssertTrue(i.Next("\xC3\xA5\xE2\x9C\x94"));
	JAssertTrue(i.AtEnd());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(7, j);
	JAssertFalse(i.GetNextCharacterIndex(&j));
	const JStringMatch& m1 = i.GetLastMatch();
	JAssertEqual(2, m1.GetCharacterCount());
	JAssertEqual(5, m1.GetByteCount());
	JAssertEqual(0, m1.GetSubstringCount());
	JAssertStringsEqual("\xC3\xA5\xE2\x9C\x94", m1.GetString());
	JString matchString = m1.GetString();
	JAssertFalse(matchString.IsOwner());

	JAssertTrue(i.Prev("3\xC2\xA9"));
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(2, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(3, j);
	const JStringMatch& m2 = i.GetLastMatch();
	JAssertEqual(2, m2.GetCharacterCount());
	JAssertEqual(3, m2.GetByteCount());
	JAssertEqual(0, m2.GetSubstringCount());
	JAssertStringsEqual("3\xC2\xA9", m2.GetString());

	JAssertTrue(i.Prev(&c));
	JAssertEqual('2', c);
}

JTEST(Regex)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(s);
	JUtf8Character c;
	JIndex j;

	JRegex r = ".3";

	JAssertTrue(i.Next(r));
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(3, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(4, j);
	const JStringMatch& m1 = i.GetLastMatch();
	JAssertEqual(2, m1.GetCharacterCount());
	JAssertEqual(2, m1.GetByteCount());
	JAssertEqual(0, m1.GetSubstringCount());
	JAssertStringsEqual("23", m1.GetString());

	r.SetPattern("(\xC3\x85)" "(.)");

	JAssertTrue(i.Next(r));
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(7, j);
	const JStringMatch& m2 = i.GetLastMatch();
	JAssertEqual(2, m2.GetCharacterCount());
	JAssertEqual(4, m2.GetByteCount());
	JAssertEqual(2, m2.GetSubstringCount());
	JAssertStringsEqual("\xC3\x85\xC3\xA5", m2.GetString());
	JAssertStringsEqual("\xC3\x85", m2.GetSubstring(1));
	JAssertStringsEqual("\xC3\xA5", m2.GetSubstring(2));
	JString matchString = m2.GetSubstring(2);
	JAssertFalse(matchString.IsOwner());

	i.MoveTo(kJIteratorStartAtEnd, 0);
	JAssertTrue(i.Prev(r));
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(4, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(5, j);
	const JStringMatch& m3 = i.GetLastMatch();
	JAssertEqual(2, m3.GetCharacterCount());
	JAssertEqual(4, m3.GetByteCount());
	JAssertEqual(2, m3.GetSubstringCount());
	JAssertStringsEqual("\xC3\x85\xC3\xA5", m3.GetString());
	JAssertStringsEqual("\xC3\x85", m3.GetSubstring(1));
	JAssertStringsEqual("\xC3\xA5", m3.GetSubstring(2));
}

JTEST(Accumulate)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(s);
	JUtf8Character c;

	JAssertTrue(i.Next("\xC2\xA9"));
	i.BeginMatch();
	JAssertTrue(i.SkipNext(2));
	const JStringMatch& m1 = i.FinishMatch();
	JAssertEqual(2, m1.GetCharacterCount());
	JAssertEqual(4, m1.GetByteCount());
	JAssertStringsEqual("\xC3\x85\xC3\xA5", m1.GetString());

	i.MoveTo(kJIteratorStartAtBeginning, 0);
	i.BeginMatch();
	i.SkipNext(5);
	const JStringMatch& m2 = i.FinishMatch();
	JAssertEqual(5, m2.GetCharacterCount());
	JAssertEqual(7, m2.GetByteCount());
	JAssertStringsEqual("123\xC2\xA9\xC3\x85", m2.GetString());

	i.MoveTo(kJIteratorStartAtBeginning, 0);
	i.BeginMatch();
	JAssertTrue(i.Next("\xC3\x85"));
	const JStringMatch& m3 = i.FinishMatch();
	JAssertEqual(4, m3.GetCharacterCount());
	JAssertEqual(5, m3.GetByteCount());
	JAssertStringsEqual("123\xC2\xA9", m3.GetString());

	i.MoveTo(kJIteratorStartAtEnd, 0);
	i.BeginMatch();
	JAssertTrue(i.Prev("\xC3\x85"));
	const JStringMatch& m4 = i.FinishMatch();
	JAssertEqual(2, m4.GetCharacterCount());
	JAssertEqual(5, m4.GetByteCount());
	JAssertStringsEqual("\xC3\xA5\xE2\x9C\x94", m4.GetString());

	i.MoveTo(kJIteratorStartAtEnd, 0);
	i.BeginMatch();
	JAssertTrue(i.Prev("\xC3\x85"));
	const JStringMatch& m5 = i.FinishMatch(kJFalse);
	JAssertEqual(3, m5.GetCharacterCount());
	JAssertEqual(7, m5.GetByteCount());
	JAssertStringsEqual("\xC3\x85\xC3\xA5\xE2\x9C\x94", m5.GetString());
}

JTEST(Set)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(&s);
	JUtf8Character c;
	JIndex j;

	JAssertEqual(12, s.GetByteCount());

	i.MoveTo(kJIteratorStartBefore, 5);
	JAssertTrue(i.SetNext('s'));
	JAssertTrue(i.SetNext('Q'));
	JAssertTrue(i.Next(&c, kJFalse));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
	JAssertEqual(7, s.GetCharacterCount());
	JAssertEqual(10, s.GetByteCount());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(7, j);

	JAssertTrue(i.Prev(&c, kJFalse));
	JAssertEqual('Q', c);
	JAssertTrue(i.SetPrev("\xE2\x9C\x94"));
	JAssertTrue(i.SetPrev("\xE2\x9C\x94"));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC2\xA9", c.GetBytes());
	JAssertEqual(7, s.GetCharacterCount());
	JAssertEqual(14, s.GetByteCount());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(3, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(4, j);
}

JTEST(Remove)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(&s);
	JUtf8Character c;
	JIndex j;

	JAssertEqual(7, s.GetCharacterCount());
	JAssertEqual(12, s.GetByteCount());

	i.MoveTo(kJIteratorStartBefore, 5);
	JAssertTrue(i.RemovePrev(2));
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(2, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(3, j);
	JAssertTrue(i.Prev(&c, kJFalse));
	JAssertEqual('2', c);
	JAssertEqual(5, s.GetCharacterCount());
	JAssertEqual(9, s.GetByteCount());

	JAssertTrue(i.RemoveNext(2));
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(2, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(3, j);
	JAssertTrue(i.Next(&c, kJFalse));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
	JAssertEqual(3, s.GetCharacterCount());
	JAssertEqual(5, s.GetByteCount());

	JAssertFalse(i.RemoveNext(2));
	JAssertTrue(i.AtEnd());
	JAssertEqual(2, s.GetCharacterCount());
	JAssertEqual(2, s.GetByteCount());

	JAssertFalse(i.RemovePrev(3));
	JAssertTrue(i.AtBeginning());
	JAssertEqual(0, s.GetCharacterCount());
	JAssertEqual(0, s.GetByteCount());
}

JTEST(RemoveLastMatch)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(&s);
	JUtf8Character c;
	JIndex j;

	JAssertTrue(i.Next("\xC3\xA5\xE2\x9C\x94"));
	JAssertTrue(i.AtEnd());
	i.RemoveLastMatch();
	JAssertTrue(i.AtEnd());
	JAssertEqual(5, s.GetCharacterCount());
	JAssertEqual(7, s.GetByteCount());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(5, j);
	JAssertFalse(i.GetNextCharacterIndex(&j));

	JAssertTrue(i.Prev("3\xC2\xA9"));
	i.RemoveLastMatch();
	JAssertEqual(3, s.GetCharacterCount());
	JAssertEqual(4, s.GetByteCount());
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(2, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(3, j);
}

JTEST(RemoveAll)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(&s);
	JUtf8Character c;
	JIndex j;

	JAssertTrue(i.SkipNext(5));
	i.RemoveAllPrev();
	JAssertEqual(2, s.GetCharacterCount());
	i.RemoveAllNext();
	JAssertTrue(s.IsEmpty());
	JAssertFalse(i.GetPrevCharacterIndex(&j));
	JAssertFalse(i.GetPrevCharacterIndex(&j));
}

JTEST(ReplaceLastMatch)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(&s);
	JUtf8Character c;
	JIndex j;

	JAssertTrue(i.Next("\xC3\xA5\xE2\x9C\x94"));
	JAssertTrue(i.AtEnd());
	i.ReplaceLastMatch("456");
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(8, j);
	JAssertTrue(i.Prev(&c, kJFalse));
	JAssertEqual('6', c);
	JAssertEqual(8, s.GetCharacterCount());
	JAssertEqual(10, s.GetByteCount());

	JAssertTrue(i.Prev("3\xC2\xA9"));
	i.ReplaceLastMatch("\xE2\x9C\x94");
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(2, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(3, j);
	JAssertTrue(i.Prev(&c, kJFalse));
	JAssertEqual('2', c);
	JAssertTrue(i.Next(&c, kJFalse));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
	JAssertEqual(7, s.GetCharacterCount());
	JAssertEqual(10, s.GetByteCount());
}

JTEST(ReplaceLastMatchPositionedInside)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(&s);
	JUtf8Character c;
	JIndex j;

	JAssertTrue(i.Next("\xC3\xA5\xE2\x9C\x94"));
	i.SkipPrev();
	i.ReplaceLastMatch("456");
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(5, j);
	JAssertTrue(i.Prev(&c, kJFalse));
	JAssertStringsEqual("\xC3\x85", c.GetBytes());
	JAssertEqual(8, s.GetCharacterCount());
	JAssertEqual(10, s.GetByteCount());
}

JTEST(ReplaceLastMatchPositionedAfter)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(&s);
	JUtf8Character c;
	JIndex j;

	JAssertTrue(i.Next("\xC3\x85\xC3\xA5"));
	i.SkipNext();
	i.ReplaceLastMatch("456");
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(8, j);
	JAssertTrue(i.Prev(&c, kJFalse));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
	JAssertEqual(8, s.GetCharacterCount());
	JAssertEqual(11, s.GetByteCount());
}

JTEST(Insert)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);
	JStringIterator i(&s);
	JUtf8Character c;
	JIndex j;

	i.MoveTo(kJIteratorStartBefore, 5);
	i.Insert("\xE2\x9C\x94" "5");
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xE2\x9C\x94", c.GetBytes());
	JAssertTrue(i.Next(&c));
	JAssertEqual('5', c);
	JAssertTrue(i.GetPrevCharacterIndex(&j));
	JAssertEqual(6, j);
	JAssertTrue(i.GetNextCharacterIndex(&j));
	JAssertEqual(7, j);
}

JTEST(SwitchIterators)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94", 0);

	{
	JStringIterator i(s);
	JAssertTrue(i.AtBeginning());
	}

	{
	JStringIterator i(s, kJIteratorStartAtEnd);
	JAssertTrue(i.AtEnd());
	}
}

JTEST(Empty)
{
	JString s;
	JStringIterator i(s);
	JAssertTrue(i.IsValid());
	JAssertTrue(i.AtBeginning());
	JAssertTrue(i.AtEnd());
}

JTEST(Invalid)
{
	JString* s1 = jnew JString();
	assert( s1 != NULL );

	JStringIterator i1(s1);

	jdelete s1;

	JAssertFalse(i1.IsValid());

	JString s2("abc", 0);
	JStringIterator i2(s2);
	s2 = "xyz";
	JAssertFalse(i2.IsValid());

	JString s3("abc", 0);
	JStringIterator i3(s3);
	s3.Clear();
	JAssertFalse(i3.IsValid());
}
