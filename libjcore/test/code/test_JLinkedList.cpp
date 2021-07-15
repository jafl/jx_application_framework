/******************************************************************************
 test_JLinkedList.cpp

	Program to test JLinkedList and JLinkedListIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include "jListTestUtil.h"
#include <JLinkedList.h>
#include <JBroadcastTester.h>
#include <algorithm>
#include <numeric>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Construct)
{
	JLinkedList<long> a;

	JAssertTrue(a.IsEmpty());
	JAssertEqual(0, a.GetElementCount());
	JAssertFalse(a.IndexValid(1));

	JSetList("1 2 3 4 5", &a);

	JAssertFalse(a.IsEmpty());
	JAssertEqual(5, a.GetElementCount());
	JAssertTrue(a.IndexValid(1));
	JAssertTrue(a.IndexValid(5));
	JAssertFalse(a.IndexValid(6));
	JAssertEqual(5, a.GetIndexFromEnd(1));
	JAssertEqual(4, a.GetIndexFromEnd(2));
	JAssertEqual(1, a.GetIndexFromEnd(5));

	const JLinkedList<long> a2 = a;
	verify("1 2 3 4 5", a2);

	JLinkedList<long> a3;
	JAssertTrue(a3.IsEmpty());
	a3 = a;
	verify("1 2 3 4 5", a3);
}

JTEST(Prepend)
{
	JLinkedList<long> a;
	JBroadcastTester snoop(&a);

	for (long j : { 5,4,3,2,1 })
		{
		snoop.Expect(JListT::kElementsInserted,
			[] (const JBroadcaster::Message& m)
			{
				const JListT::ElementsInserted* ei =
					dynamic_cast<const JListT::ElementsInserted*>(&m);
				JAssertNotNull(ei);
				JAssertEqual(1, ei->GetFirstIndex());
				JAssertEqual(1, ei->GetLastIndex());
				JAssertEqual(1, ei->GetCount());
			});

		a.PrependElement(j);
		}

	verify("1 2 3 4 5", a);

	JAssertEqual(1, a.GetFirstElement());
	JAssertEqual(5, a.GetLastElement());
}

JTEST(Append)
{
	JLinkedList<long> a;
	JBroadcastTester snoop(&a);

	for (long j : { 1,2,3,4,5 })
		{
		snoop.Expect(JListT::kElementsInserted,
			[j] (const JBroadcaster::Message& m)
			{
				const JListT::ElementsInserted* ei =
					dynamic_cast<const JListT::ElementsInserted*>(&m);
				JAssertNotNull(ei);
				JAssertEqual(j, ei->GetFirstIndex());
				JAssertEqual(j, ei->GetLastIndex());
				JAssertEqual(1, ei->GetCount());
				JAssertFalse(ei->Contains(j-1));
				JAssertTrue(ei->Contains(j));
				JAssertFalse(ei->Contains(j+1));

				JIndex k = j-1;
				ei->AdjustIndex(&k);
				JAssertEqual(j-1, k);

				k = j;
				ei->AdjustIndex(&k);
				JAssertEqual(j+1, k);

				k = j+1;
				ei->AdjustIndex(&k);
				JAssertEqual(j+2, k);
			});

		a.AppendElement(j);
		}

	verify("1 2 3 4 5", a);

	JAssertEqual(1, a.GetFirstElement());
	JAssertEqual(5, a.GetLastElement());
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

JTEST(Iterator)
{
	JLinkedList<long> a;
	JSetList("5 4 3 2 1", &a);

	JListIterator<long>* iter = a.NewIterator(kJIteratorStartAtEnd);
	JAssertFalse(iter->AtBeginning());
	JAssertTrue(iter->AtEnd());

	long j;
	JAssertFalse(iter->Next(&j));

	JIndex i;
	JAssertFalse(iter->GetNextElementIndex(&i));
	JAssertTrue(iter->GetPrevElementIndex(&i));
	JAssertEqual(5, i);

	JAssertTrue(iter->Prev(&j, kJFalse));
	JAssertEqual(1, j);
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(1, j);
	JAssertEqual(4, iter->GetPrevElementIndex());
	JAssertEqual(5, iter->GetNextElementIndex());
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(2, j);
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(3, j);
	JAssertEqual(2, iter->GetPrevElementIndex());
	JAssertEqual(3, iter->GetNextElementIndex());
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(4, j);
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(5, j);

	JAssertFalse(iter->Prev(&j));
	JAssertTrue(iter->AtBeginning());
	JAssertFalse(iter->AtEnd());

	JAssertFalse(iter->GetPrevElementIndex(&i));
	JAssertTrue(iter->GetNextElementIndex(&i));
	JAssertEqual(1, i);

	JAssertTrue(iter->Next(&j, kJFalse));
	JAssertEqual(5, j);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(5, j);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(4, j);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(3, j);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(2, j);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(1, j);

	JAssertFalse(iter->Next(&j));
	JAssertFalse(iter->AtBeginning());
	JAssertTrue(iter->AtEnd());

	iter->SkipNext(10);
	iter->SkipPrev(3);
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(4, j);

	iter->SkipNext(2);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(2, j);

	jdelete iter;
}

JTEST(IteratorConst)
{
	JLinkedList<long> a;
	JSetList("5 4 3 2 1", &a);

	const JLinkedList<long>& b = a;

	JListIterator<long>* iter = b.NewIterator(kJIteratorStartAfter, 2);

	JAssertFalse(iter->SetPrev(1));
	JAssertFalse(iter->SetNext(1));
	JAssertFalse(iter->RemovePrev());
	JAssertFalse(iter->RemoveNext());
	JAssertFalse(iter->Insert(5));

	jdelete iter;
}

JTEST(IteratorModification)
{
	JLinkedList<long> a;
	JSetList("5 4 3 2 1", &a);

	JListIterator<long>* iter = a.NewIterator();
	JAssertEqual(1, iter->GetNextElementIndex());

	long j;

	a.PrependElement(-1);
	JAssertEqual(1, iter->GetNextElementIndex());
	verify("-1 5 4 3 2 1", a);

	JAssertTrue(iter->Next(&j));
	JAssertEqual(-1, j);
	JAssertEqual(2, iter->GetNextElementIndex());

	a.PrependElement(-2);
	JAssertEqual(3, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(5, j);
	verify("-2 -1 5 4 3 2 1", a);

	iter->MoveTo(kJIteratorStartAfter, 4);
	a.AppendElement(-3);
	JAssertEqual(5, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(3, j);
	iter->SkipPrev();
	verify("-2 -1 5 4 3 2 1 -3", a);

	{
	JListIterator<long>* i2 = a.NewIterator(kJIteratorStartBefore, 2);
	i2->Insert(-4);
	jdelete i2;
	}

	JAssertEqual(6, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(3, j);
	iter->SkipPrev();
	verify("-2 -4 -1 5 4 3 2 1 -3", a);

	iter->SetPrev(-4, kJFalse);
	iter->SetNext(-3, kJFalse);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(-3, j);
	iter->SkipPrev();
	verify("-2 -4 -1 5 -4 -3 2 1 -3", a);

	iter->RemovePrev(2);
	iter->RemoveNext(1);
	JAssertEqual(4, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(2, j);
	iter->SkipPrev();
	verify("-2 -4 -1 2 1 -3", a);

	iter->Insert(7);
	JAssertEqual(4, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(7, j);
	iter->SkipPrev();
	verify("-2 -4 -1 7 2 1 -3", a);

	iter->MoveTo(kJIteratorStartAtEnd, 0);
	iter->RemovePrev(2);
	iter->RemoveNext();
	verify("-2 -4 -1 7 2", a);

	iter->MoveTo(kJIteratorStartAfter, 2);
	iter->RemovePrev(3);
	iter->RemoveNext();
	verify("7 2", a);

	iter->MoveTo(kJIteratorStartAtBeginning, 0);
	iter->SetNext(5);
	iter->SetNext(4);
	verify("5 4", a);

	iter->SetPrev(3);
	iter->SetPrev(2);
	verify("2 3", a);

	a.RemoveAll();
	iter->Insert(1);
	verify("1", a);

	iter->RemoveNext();
	iter->Insert(2);
	verify("2", a);

	iter->SkipNext();
	iter->RemovePrev();
	iter->Insert(3);
	verify("3", a);

	jdelete iter;
}

JTEST(TwoIterators)
{
	JLinkedList<long> a;
	JSetList("5 4 3 2 1", &a);

	JListIterator<long>* i1 = a.NewIterator(kJIteratorStartBefore, 3);
	JListIterator<long>* i2 = a.NewIterator(kJIteratorStartAtEnd);

	long j;

	i1->Insert(-1);
	JAssertEqual(3, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(-1, j);
	i1->SkipPrev();
	JAssertEqual(6, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("5 4 -1 3 2 1", a);

	i1->RemoveNext(2);
	JAssertEqual(3, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(2, j);
	i1->SkipPrev();
	JAssertEqual(4, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("5 4 2 1", a);

	i2->Insert(-2);
	JAssertEqual(3, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(2, j);
	i1->SkipPrev();
	JAssertEqual(4, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("5 4 2 1 -2", a);

	a.PrependElement(-3);
	JAssertEqual(4, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(2, j);
	i1->SkipPrev();
	JAssertEqual(5, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("-3 5 4 2 1 -2", a);

	{
	JListIterator<long>* i3 = a.NewIterator(kJIteratorStartBefore, 4);
	i3->Insert(-4);
	jdelete i3;
	}

	JAssertEqual(4, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(-4, j);
	i1->SkipPrev();
	JAssertEqual(6, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("-3 5 4 -4 2 1 -2", a);

	jdelete i1;
	jdelete i2;
}
