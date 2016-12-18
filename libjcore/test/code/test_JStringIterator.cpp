/******************************************************************************
 test_JStringIterator.cc

	Program to test JStringIterator class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <locale.h>
#include <jassert_simple.h>

int main()
{
	setlocale(LC_ALL, "");
	return JUnitTestManager::Execute();
}

JTEST(Iteration)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");

	JStringIterator i(s);
	JAssertTrue(i.IsValid());
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());

	JUtf8Character c;
	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);
	JAssertTrue(i.Next(&c));
	JAssertEqual('2', c);
	JAssertTrue(i.Next(&c));
	JAssertEqual('3', c);
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC2\xA9", JString(c));
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));
	JAssertFalse(i.Next(&c));
	JAssertFalse(i.AtBeginning());
	JAssertTrue(i.AtEnd());

	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC2\xA9", JString(c));
	JAssertTrue(i.Prev(&c));
	JAssertEqual('3', c);
	JAssertTrue(i.Prev(&c));
	JAssertEqual('2', c);
	JAssertTrue(i.Prev(&c));
	JAssertEqual('1', c);
	JAssertFalse(i.Prev(&c));
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());

	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);
	JAssertTrue(i.Prev(&c));
	JAssertEqual('1', c);
}

JTEST(Move)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	JStringIterator i(s);
	JUtf8Character c;

	i.MoveTo(kJIteratorStartAtEnd, 0);
	JAssertFalse(i.AtBeginning());
	JAssertTrue(i.AtEnd());
	JAssertFalse(i.Next(&c));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));

	i.MoveTo(kJIteratorStartAtBeginning, 0);
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertFalse(i.Prev(&c));
	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);

	i.MoveTo(kJIteratorStartBefore, 5);
	JAssertFalse(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));

	i.MoveTo(kJIteratorStartBefore, 1);
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);

	i.MoveTo(kJIteratorStartBeforeByte, 6);
	JAssertFalse(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));

	i.MoveTo(kJIteratorStartBeforeByte, 1);
	JAssertTrue(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.Next(&c));
	JAssertEqual('1', c);

	i.MoveTo(kJIteratorStartAfter, 6);
	JAssertFalse(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));

	i.MoveTo(kJIteratorStartAfter, 7);
	JAssertFalse(i.AtBeginning());
	JAssertTrue(i.AtEnd());
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));

	i.MoveTo(kJIteratorStartAfterByte, 9);
	JAssertFalse(i.AtBeginning());
	JAssertFalse(i.AtEnd());
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));

	i.MoveTo(kJIteratorStartAfterByte, s.GetByteCount());
	JAssertFalse(i.AtBeginning());
	JAssertTrue(i.AtEnd());
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));
}

JTEST(Skip)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	JStringIterator i(s);
	JUtf8Character c;

	i.MoveTo(kJIteratorStartAtBeginning, 0);
	i.SkipNext(5);
	JAssertTrue(i.Next(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));

	i.MoveTo(kJIteratorStartAtEnd, 0);
	i.SkipPrev(2);
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));
}

JTEST(Search)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	JStringIterator i(s);
	JUtf8Character c;

	JAssertTrue(i.Next("\xC3\xA5\xE2\x9C\x94"));
	JAssertTrue(i.AtEnd());
	const JStringMatch& m1 = i.GetLastMatch();
	JAssertEqual(2, m1.GetCharacterCount());
	JAssertEqual(5, m1.GetByteCount());
	JAssertEqual(0, m1.GetSubstringCount());
	JAssertStringsEqual("\xC3\xA5\xE2\x9C\x94", m1.GetString());

	JAssertTrue(i.Prev("3\xC2\xA9"));
	const JStringMatch& m2 = i.GetLastMatch();
	JAssertEqual(2, m2.GetCharacterCount());
	JAssertEqual(3, m2.GetByteCount());
	JAssertEqual(0, m2.GetSubstringCount());
	JAssertStringsEqual("3\xC2\xA9", m2.GetString());

	JAssertTrue(i.Prev(&c));
	JAssertEqual('2', c);
}

JTEST(Set)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");
	JStringIterator i(&s);
	JUtf8Character c;

	JAssertEqual(12, s.GetByteCount());

	i.MoveTo(kJIteratorStartBefore, 5);
	JAssertTrue(i.SetNext('s'));
	JAssertTrue(i.SetNext('Q'));
	JAssertTrue(i.Next(&c, kJFalse));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));
	JAssertEqual(7, s.GetCharacterCount());
	JAssertEqual(10, s.GetByteCount());

	JAssertTrue(i.Prev(&c, kJFalse));
	JAssertEqual('Q', c);
	JAssertTrue(i.SetPrev("\xE2\x9C\x94"));
	JAssertTrue(i.SetPrev("\xE2\x9C\x94"));
	JAssertTrue(i.Prev(&c));
	JAssertStringsEqual("\xC2\xA9", JString(c));
	JAssertEqual(7, s.GetCharacterCount());
	JAssertEqual(14, s.GetByteCount());
}

JTEST(SwitchIterators)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");

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

	JString s2 = "abc";
	JStringIterator i2(s2);
	s2 = "xyz";
	JAssertFalse(i2.IsValid());

	JString s3 = "abc";
	JStringIterator i3(s3);
	s3.Clear();
	JAssertFalse(i3.IsValid());
}
