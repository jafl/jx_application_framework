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

JTEST(ConstIteration)
{
	JString s("123\xC2\xA9\xC3\x85\xC3\xA5\xE2\x9C\x94");

	JStringIterator i1(s);
	JAssertTrue(i1.IsValid());
	JAssertTrue(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());

	JUtf8Character c;
	JAssertTrue(i1.Next(&c));
	JAssertEqual('1', c);
	JAssertTrue(i1.Next(&c));
	JAssertEqual('2', c);
	JAssertTrue(i1.Next(&c));
	JAssertEqual('3', c);
	JAssertTrue(i1.Next(&c));
	JAssertStringsEqual("\xC2\xA9", JString(c));
	JAssertTrue(i1.Next(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));
	JAssertTrue(i1.Next(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));
	JAssertTrue(i1.Next(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));
	JAssertFalse(i1.Next(&c));
	JAssertFalse(i1.AtBeginning());
	JAssertTrue(i1.AtEnd());

	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xC2\xA9", JString(c));
	JAssertTrue(i1.Prev(&c));
	JAssertEqual('3', c);
	JAssertTrue(i1.Prev(&c));
	JAssertEqual('2', c);
	JAssertTrue(i1.Prev(&c));
	JAssertEqual('1', c);
	JAssertFalse(i1.Prev(&c));
	JAssertTrue(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());

	i1.MoveTo(kJIteratorStartAtEnd, 0);
	JAssertFalse(i1.AtBeginning());
	JAssertTrue(i1.AtEnd());
	JAssertFalse(i1.Next(&c));
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));

	i1.MoveTo(kJIteratorStartAtBeginning, 0);
	JAssertTrue(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());
	JAssertFalse(i1.Prev(&c));
	JAssertTrue(i1.Next(&c));
	JAssertEqual('1', c);

	i1.MoveTo(kJIteratorStartBefore, 5);
	JAssertFalse(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());
	JAssertTrue(i1.Next(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));

	i1.MoveTo(kJIteratorStartBefore, 1);
	JAssertTrue(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());
	JAssertTrue(i1.Next(&c));
	JAssertEqual('1', c);

	i1.MoveTo(kJIteratorStartBeforeByte, 6);
	JAssertFalse(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());
	JAssertTrue(i1.Next(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));

	i1.MoveTo(kJIteratorStartBeforeByte, 1);
	JAssertTrue(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());
	JAssertTrue(i1.Next(&c));
	JAssertEqual('1', c);

	i1.MoveTo(kJIteratorStartAfter, 6);
	JAssertFalse(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));

	i1.MoveTo(kJIteratorStartAfter, 7);
	JAssertFalse(i1.AtBeginning());
	JAssertTrue(i1.AtEnd());
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));

	i1.MoveTo(kJIteratorStartAfterByte, 9);
	JAssertFalse(i1.AtBeginning());
	JAssertFalse(i1.AtEnd());
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));

	i1.MoveTo(kJIteratorStartAfterByte, s.GetByteCount());
	JAssertFalse(i1.AtBeginning());
	JAssertTrue(i1.AtEnd());
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xE2\x9C\x94", JString(c));

	i1.MoveTo(kJIteratorStartAtBeginning, 0);
	i1.SkipNext(5);
	JAssertTrue(i1.Next(&c));
	JAssertStringsEqual("\xC3\xA5", JString(c));

	i1.MoveTo(kJIteratorStartAtEnd, 0);
	i1.SkipPrev(2);
	JAssertTrue(i1.Prev(&c));
	JAssertStringsEqual("\xC3\x85", JString(c));
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
	JAssertTrue(i.AtBeginning());
	JAssertTrue(i.AtEnd());
}
