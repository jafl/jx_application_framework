/******************************************************************************
 test_JLinkedList.cpp

	Program to test JLinkedList and JLinkedListIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JLinkedList.h>
#include <JBroadcastTester.h>
#include <algorithm>
#include <numeric>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JListT::CompareResult
CompareLongs
	(
	const long& a,
	const long& b
	)
{
	if (a < b)
		{
		return JListT::kFirstLessSecond;
		}
	else if (a == b)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
		}
}

JTEST(Exercise)
{
	JLinkedList<long> a1;
	JBroadcastTester snoop1(&a1);
	JAssertTrue(a1.IsEmpty());
	JAssertEqual(0, a1.GetElementCount());

	long i;
	for (long j : { 1,2,3,4,5 })
		{
		snoop1.Expect(JListT::kElementsInserted);
		a1.AppendElement(j);
		}

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(5, a1.GetElementCount());

	JLinkedListIterator<long> iter(&a1);

	snoop1.Expect(JListT::kElementsRemoved);
	{
	long expect[] = { 1,2,3,5 };
	long j        = 0;
	while (iter.Next(&i))
		{
		JAssertEqual(expect[j], i);
		j++;

		if (i == 3) iter.RemoveNext();
		}
	JAssertEqual(4, j);
	}
	snoop1.Expect(JListT::kElementsRemoved);
	{
	long expect[] = { 1,3,5 };
	long j        = 0;
	while (iter.Prev(&i))
		{
		JAssertEqual(expect[j], i);
		j++;

		if (i == 3) iter.RemovePrev();
		}
	JAssertEqual(3, j);
	}
	JAssertFalse(a1.IsEmpty());
	JAssertEqual(3, a1.GetElementCount());

	JLinkedList<long> a2 = a1;
{
	JBroadcastTester snoop2(&a2);

	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetElementCount());

	JLinkedListIterator<long> iter2(&a2, kJIteratorStartAtEnd);
	{
	long expect[] = { 1, 3, 5, 1 };
	long j        = 0;
	while (iter2.Prev(&i))
		{
		JAssertEqual(expect[j], i);
		j++;
		}
	JAssertEqual(4, j);
	}
	snoop2.Expect(JListT::kElementsRemoved);

	a2.RemoveAll();
	JAssertTrue(a2.IsEmpty());
	JAssertEqual(0, a2.GetElementCount());

	a2 = a1;
	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetElementCount());

	iter2.MoveTo(kJIteratorStartAtEnd, 0);
	{
	long expect[] = { 1, 3, 5, 1 };
	long j        = 0;
	while (iter2.Prev(&i))
		{
		JAssertEqual(expect[j], i);
		j++;
		}
	JAssertEqual(4, j);
	}
	snoop2.Expect(JListT::kElementsInserted);
	a2.AppendElement(1);
}
	JLinkedListIterator<long> iter2(&a2);
}

JTEST(RangeBasedForLoop)
{
	JLinkedList<long> a;
	a.AppendElement(3);
	a.AppendElement(2);
	a.AppendElement(5);

	long b[3];
	int j = 0;
	for (long i : a)
		{
		b[j++] = i;
		}

	JAssertEqual(3, b[0]);
	JAssertEqual(2, b[1]);
	JAssertEqual(5, b[2]);
}

JTEST(FunctionalProgramming)
{
	JLinkedList<long> a;
	a.AppendElement(3);
	a.AppendElement(2);
	a.AppendElement(5);

	long sum = std::accumulate(begin(a), end(a), 0);
	JAssertEqual(10, sum);

	sum = 0;
	std::for_each(begin(a), end(a), [&sum](long v){ sum += v; });
	JAssertEqual(10, sum);
}
