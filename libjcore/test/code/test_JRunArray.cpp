/******************************************************************************
 test_JRunArray.cpp

	Program to test JRunArray and JRunArrayIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JRunArray.h>
#include <JBroadcastTester.h>
#include <jTestUtil.h>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <jAssert.h>

#define verify(str, list)	JAssertStringsEqual(str, JPrintList(list));

int main()
{
	return JTestManager::Execute();
}

JInteger
GetValue
	(
	const long&	data
	)
{
	return data;
}

JTEST(Construct)
{
	JRunArray<long> a;

	JAssertTrue(a.IsEmpty());
	JAssertEqual(0, a.GetElementCount());
	JAssertFalse(a.IndexValid(1));

	JSetList("1 2 3 4 5", &a);

	JAssertFalse(a.IsEmpty());
	JAssertEqual(5, a.GetElementCount());
	JAssertEqual(5, a.GetRunCount());
	JAssertTrue(a.IndexValid(1));
	JAssertTrue(a.IndexValid(5));
	JAssertFalse(a.IndexValid(6));
	JAssertEqual(5, a.GetIndexFromEnd(1));
	JAssertEqual(4, a.GetIndexFromEnd(2));
	JAssertEqual(1, a.GetIndexFromEnd(5));

	const JRunArray<long> a2 = a;
	verify("1 2 3 4 5", a2);

	JRunArray<long> a3;
	JAssertTrue(a3.IsEmpty());
	a3 = a;
	verify("1 2 3 4 5", a3);

	a3.RemoveAll();
	JAssertTrue(a3.IsEmpty());
}

JTEST(Prepend)
{
	JRunArray<long> a;
	JBroadcastTester snoop(&a);

	for (long j : { 5,4,4,2,1 })
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

	verify("1 2 4 4 5", a);

	JAssertEqual(4, a.GetRunCount());
	JAssertEqual(1, a.GetFirstElement());
	JAssertEqual(5, a.GetLastElement());
}

JTEST(Append)
{
	JRunArray<long> a;
	JBroadcastTester snoop(&a);

	long n = 0;
	for (long j : { 1,3,3,4,5 })
		{
		n++;
		snoop.Expect(JListT::kElementsInserted,
			[n] (const JBroadcaster::Message& m)
			{
				const JListT::ElementsInserted* ei =
					dynamic_cast<const JListT::ElementsInserted*>(&m);
				JAssertNotNull(ei);
				JAssertEqual(n, ei->GetFirstIndex());
				JAssertEqual(n, ei->GetLastIndex());
				JAssertEqual(1, ei->GetCount());
				JAssertFalse(ei->Contains(n-1));
				JAssertTrue(ei->Contains(n));
				JAssertFalse(ei->Contains(n+1));

				JIndex k = n-1;
				ei->AdjustIndex(&k);
				JAssertEqual(n-1, k);

				k = n;
				ei->AdjustIndex(&k);
				JAssertEqual(n+1, k);

				k = n+1;
				ei->AdjustIndex(&k);
				JAssertEqual(n+2, k);
			});

		a.AppendElement(j);
		}

	verify("1 3 3 4 5", a);

	JAssertEqual(4, a.GetRunCount());
	JAssertEqual(1, a.GetFirstElement());
	JAssertEqual(5, a.GetLastElement());
}

JTEST(Runs)
{
	JRunArray<long> a;
	JSetList("1 2 3 4 5", &a);

	JListIterator<long>* iter = a.NewIterator();

	iter->MoveTo(kJIteratorStartBefore, 5);
	iter->Insert(5);
	iter->Insert(4);
	verify("1 2 3 4 4 5 5", a);

	iter->MoveTo(kJIteratorStartBefore, 3);
	iter->SetNext(4);
	verify("1 2 4 4 4 5 5", a);

	iter->MoveTo(kJIteratorStartAfter, 3);
	iter->SetNext(2);
	verify("1 2 4 2 4 5 5", a);

	iter->SetNext(4);
	verify("1 2 4 4 4 5 5", a);

	JSetList("-1 1 1 1 1 2 3 3 3 4 5 5 10", &a);

	JRunArrayIterator<long>* runiter = dynamic_cast<JRunArrayIterator<long>*>(iter);

	runiter->MoveTo(kJIteratorStartBefore, 3);
	runiter->Insert(1, 2);
	verify("-1 1 1 1 1 1 1 2 3 3 3 4 5 5 10", a);

	runiter->MoveTo(kJIteratorStartBefore, 2);
	runiter->Insert(-1, 2);
	verify("-1 -1 -1 1 1 1 1 1 1 2 3 3 3 4 5 5 10", a);

	runiter->MoveTo(kJIteratorStartBefore, 4);
	runiter->Insert(0, 2);
	verify("-1 -1 -1 0 0 1 1 1 1 1 1 2 3 3 3 4 5 5 10", a);

	runiter->MoveTo(kJIteratorStartBefore, 11);
	runiter->Insert(2, 2);
	verify("-1 -1 -1 0 0 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10", a);

	a.AppendElements(10, 2);
	verify("-1 -1 -1 0 0 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10 10 10", a);

	a.AppendElements(11, 2);
	verify("-1 -1 -1 0 0 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 4);
	runiter->SetNext(-1);
	verify("-1 -1 -1 -1 0 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 5);
	runiter->SetNext(-1);
	verify("-1 -1 -1 -1 -1 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 13);
	runiter->SetNext(2);
	verify("-1 -1 -1 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 4 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 18);
	runiter->SetNext(1);
	verify("-1 -1 -1 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 1 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 18);
	runiter->SetNext(5);
	verify("-1 -1 -1 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartAtBeginning, 0);
	runiter->SetNext(0);
	verify("0 -1 -1 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 3);
	runiter->SetNext(0);
	verify("0 -1 0 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartAfter, 10);
	runiter->SetPrev(-1, 3);
	verify("0 -1 0 -1 -1 1 1 -1 -1 -1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->SetNext(-2, 3);
	verify("0 -1 0 -1 -1 1 1 -1 -1 -1 -2 -2 -2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	long j;
	iter->Next(&j);
	JAssertEqual(-2, j);
	iter->Next(&j);
	JAssertEqual(-2, j);
	iter->Next(&j);
	JAssertEqual(-2, j);
	iter->Next(&j);
	JAssertEqual(2, j);

	JSetList("-1 -1 -1 -1 0 0 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11", &a);

	runiter->MoveTo(kJIteratorStartAfter, 5);
	runiter->RemovePrev(2);
	verify("-1 -1 -1 0 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 5);
	runiter->RemoveNext(2);
	verify("-1 -1 -1 0 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartAfter, 9);
	runiter->RemovePrev(2);
	verify("-1 -1 -1 0 1 1 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartAtBeginning, 0);
	runiter->RemoveNext(4);
	verify("1 1 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 3);
	runiter->Insert(3, 2);
	verify("1 1 3 3 2 2 3 3 3 5 5 5 10 10 10 11 11", a);

	runiter->MoveTo(kJIteratorStartBefore, 5);
	runiter->RemoveNext(6);
	verify("1 1 3 3 5 5 10 10 10 11 11", a);

	runiter->RemoveNext(3);
	verify("1 1 3 3 10 10 11 11", a);

	runiter->Insert(5, 3);
	verify("1 1 3 3 5 5 5 10 10 11 11", a);

	runiter->RemoveNext(3);
	verify("1 1 3 3 10 10 11 11", a);

	jdelete iter;
}

JTEST(CtorSlice)
{
	JRunArray<long> a1;
	a1.AppendElements(1, 3);
	a1.AppendElements(2, 3);
	a1.AppendElements(3, 3);
	a1.AppendElements(4, 3);
	a1.AppendElements(5, 3);

	JRunArray<long> a2(a1, JIndexRange(1,1));
	verify("1", a2);

	JRunArray<long> a3(a1, JIndexRange(1,3));
	verify("1 1 1", a3);

	JRunArray<long> a4(a1, JIndexRange(2,5));
	verify("1 1 2 2", a4);
}

JTEST(InsertSlice)
{
	JRunArray<long> a1;
	a1.AppendElements(1, 3);
	a1.AppendElements(2, 3);
	a1.AppendElements(3, 3);
	a1.AppendElements(4, 3);
	a1.AppendElements(5, 3);

	JRunArray<long> a2;
	JRunArrayIterator<long> iter2(&a2);

	JRunArray<long> a3;
	a3.AppendElements(1, 3);
	a3.AppendElements(2, 3);
	a3.AppendElements(3, 3);
	a3.AppendElements(4, 3);
	a3.AppendElements(5, 3);

	a2 = a3;
	a2.PrependSlice(a1, JIndexRange(1, 1));
	verify("1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	a2.PrependSlice(a1, JIndexRange(1, 2));
	verify("1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	a2.PrependSlice(a1, JIndexRange(1, 3));
	verify("1 1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	a2.PrependSlice(a1, JIndexRange(1, 4));
	verify("1 1 1 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	a2.PrependSlice(a1, JIndexRange(1, 5));
	verify("1 1 1 2 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	a2.PrependSlice(a1, JIndexRange(1, 6));
	verify("1 1 1 2 2 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	iter2.InsertSlice(a1, JIndexRange(1, 7));
	verify("1 1 1 2 2 2 3 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 2);
	iter2.InsertSlice(a1, JIndexRange(1, 1));
	verify("1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 2);
	iter2.InsertSlice(a1, JIndexRange(1, 2));
	verify("1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 2);
	iter2.InsertSlice(a1, JIndexRange(1, 3));
	verify("1 1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 2);
	iter2.InsertSlice(a1, JIndexRange(1, 4));
	verify("1 1 1 1 2 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 2);
	iter2.InsertSlice(a1, JIndexRange(1, 5));
	verify("1 1 1 1 2 2 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 2);
	iter2.InsertSlice(a1, JIndexRange(1, 6));
	verify("1 1 1 1 2 2 2 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 2);
	iter2.InsertSlice(a1, JIndexRange(1, 7));
	verify("1 1 1 1 2 2 2 3 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 3);
	iter2.InsertSlice(a1, JIndexRange(1, 1));
	verify("1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 3);
	iter2.InsertSlice(a1, JIndexRange(1, 2));
	verify("1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 3);
	iter2.InsertSlice(a1, JIndexRange(1, 3));
	verify("1 1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 3);
	iter2.InsertSlice(a1, JIndexRange(1, 4));
	verify("1 1 1 1 1 2 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 3);
	iter2.InsertSlice(a1, JIndexRange(1, 5));
	verify("1 1 1 1 1 2 2 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 3);
	iter2.InsertSlice(a1, JIndexRange(1, 6));
	verify("1 1 1 1 1 2 2 2 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 3);
	iter2.InsertSlice(a1, JIndexRange(1, 7));
	verify("1 1 1 1 1 2 2 2 3 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 4);
	iter2.InsertSlice(a1, JIndexRange(1, 1));
	verify("1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 4);
	iter2.InsertSlice(a1, JIndexRange(1, 2));
	verify("1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 4);
	iter2.InsertSlice(a1, JIndexRange(1, 3));
	verify("1 1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 4);
	iter2.InsertSlice(a1, JIndexRange(1, 4));
	verify("1 1 1 1 1 1 2 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 4);
	iter2.InsertSlice(a1, JIndexRange(1, 5));
	verify("1 1 1 1 1 1 2 2 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 4);
	iter2.InsertSlice(a1, JIndexRange(1, 6));
	verify("1 1 1 1 1 1 2 2 2 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 4);
	iter2.InsertSlice(a1, JIndexRange(1, 7));
	verify("1 1 1 1 1 1 2 2 2 3 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 5);
	iter2.InsertSlice(a1, JIndexRange(1, 1));
	verify("1 1 1 2 1 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 5);
	iter2.InsertSlice(a1, JIndexRange(1, 2));
	verify("1 1 1 2 1 1 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 5);
	iter2.InsertSlice(a1, JIndexRange(1, 3));
	verify("1 1 1 2 1 1 1 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 5);
	iter2.InsertSlice(a1, JIndexRange(1, 4));
	verify("1 1 1 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 5);
	iter2.InsertSlice(a1, JIndexRange(1, 5));
	verify("1 1 1 2 1 1 1 2 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 5);
	iter2.InsertSlice(a1, JIndexRange(1, 6));
	verify("1 1 1 2 1 1 1 2 2 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 5);
	iter2.InsertSlice(a1, JIndexRange(1, 7));
	verify("1 1 1 2 1 1 1 2 2 2 3 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 7);
	iter2.InsertSlice(a1, JIndexRange(1, 1));
	verify("1 1 1 2 2 2 1 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 7);
	iter2.InsertSlice(a1, JIndexRange(1, 2));
	verify("1 1 1 2 2 2 1 1 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 7);
	iter2.InsertSlice(a1, JIndexRange(1, 3));
	verify("1 1 1 2 2 2 1 1 1 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 7);
	iter2.InsertSlice(a1, JIndexRange(1, 4));
	verify("1 1 1 2 2 2 1 1 1 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 7);
	iter2.InsertSlice(a1, JIndexRange(1, 5));
	verify("1 1 1 2 2 2 1 1 1 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 7);
	iter2.InsertSlice(a1, JIndexRange(1, 6));
	verify("1 1 1 2 2 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	iter2.MoveTo(kJIteratorStartBefore, 7);
	iter2.InsertSlice(a1, JIndexRange(1, 7));
	verify("1 1 1 2 2 2 1 1 1 2 2 2 3 3 3 3 4 4 4 5 5 5", a2);

	iter2.InsertSlice(a1, JIndexRange(5, 7));
	verify("1 1 1 2 2 2 2 2 3 1 1 1 2 2 2 3 3 3 3 4 4 4 5 5 5", a2);

	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	iter2.InsertSlice(a1, JIndexRange(5, 7));
	verify("2 2 3 1 1 1 2 2 2 2 2 3 1 1 1 2 2 2 3 3 3 3 4 4 4 5 5 5", a2);

	iter2.InsertSlice(a1, JIndexRange(5, 7));
	verify("2 2 3 2 2 3 1 1 1 2 2 2 2 2 3 1 1 1 2 2 2 3 3 3 3 4 4 4 5 5 5", a2);

	a2 = a3;
	a2.AppendSlice(a1, JIndexRange(15, 15));
	verify("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 5", a2);

	a2 = a3;
	a2.AppendSlice(a1, JIndexRange(14, 15));
	verify("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 5 5", a2);

	a2 = a3;
	a2.AppendSlice(a1, JIndexRange(13, 15));
	verify("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 5 5 5", a2);

	a2 = a3;
	a2.AppendSlice(a1, JIndexRange(12, 15));
	verify("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 4 5 5 5", a2);

	a2 = a3;
	a2.AppendSlice(a1, JIndexRange(8, 15));
	verify("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 3 3 4 4 4 5 5 5", a2);
}

JTEST(AllElementsEqual)
{
	JRunArray<long> a;
	JSetList("-1 1 1 1 1 2 3 3 3 4 5 5 10", &a);

	long j;
	JAssertFalse(a.AllElementsEqual(&j));

	JSetList("1 1 1 1 1", &a);
	JAssertTrue(a.AllElementsEqual(&j));
	JAssertEqual(1, j);

	JRunArrayIterator<long> iter(a);
	JAssertTrue(iter.AtFirstRun());
	JAssertTrue(iter.AtLastRun());
}

JTEST(RangeBasedForLoop)
{
	JRunArray<long> a;
	a.AppendElement(3);
	a.AppendElement(2);
	a.AppendElement(2);
	a.AppendElement(5);

	long b[4];
	int j = 0;
	for (long i : a)
		{
		b[j++] = i;
		}

	JAssertEqual(3, b[0]);
	JAssertEqual(2, b[1]);
	JAssertEqual(2, b[2]);
	JAssertEqual(5, b[3]);
}

JTEST(Sum)
{
	JRunArray<long> a;
	JSetList("-1 1 1 1 1 2 3 3 3 4 5 5 10", &a);

	JInteger sum = a.SumElements(1,1, GetValue);
	JAssertEqual(-1, sum);
	sum = a.SumElements(1,2, GetValue);
	JAssertEqual(0, sum);
	sum = a.SumElements(1,5, GetValue);
	JAssertEqual(3, sum);
	sum = a.SumElements(1,8, GetValue);
	JAssertEqual(11, sum);
	sum = a.SumElements(2,8, GetValue);
	JAssertEqual(12, sum);
	sum = a.SumElements(4,8, GetValue);
	JAssertEqual(10, sum);
	sum = a.SumElements(4,9, GetValue);
	JAssertEqual(13, sum);
	sum = a.SumElements(6,6, GetValue);
	JAssertEqual(2, sum);

	JIndex endIndex;
	JInteger trueSum;
	JAssertTrue(a.FindPositiveSum(1, 1, &endIndex, &trueSum, GetValue));
	JAssertEqual(4, endIndex);
	JAssertEqual(1, trueSum);
	JAssertTrue(a.FindPositiveSum(3, 1, &endIndex, &trueSum, GetValue));
	JAssertEqual(6, endIndex);
	JAssertEqual(3, trueSum);
	JAssertTrue(a.FindPositiveSum(11, 1, &endIndex, &trueSum, GetValue));
	JAssertEqual(9, endIndex);
	JAssertEqual(11, trueSum);
	JAssertTrue(a.FindPositiveSum(10, 1, &endIndex, &trueSum, GetValue));
	JAssertEqual(8, endIndex);
	JAssertEqual(8, trueSum);
	JAssertTrue(a.FindPositiveSum(9, 2, &endIndex, &trueSum, GetValue));
	JAssertEqual(8, endIndex);
	JAssertEqual(9, trueSum);
	JAssertTrue(a.FindPositiveSum(10, 2, &endIndex, &trueSum, GetValue));
	JAssertEqual(8, endIndex);
	JAssertEqual(9, trueSum);
	JAssertTrue(a.FindPositiveSum(15, 2, &endIndex, &trueSum, GetValue));
	JAssertEqual(10, endIndex);
	JAssertEqual(15, trueSum);
	JAssertTrue(a.FindPositiveSum(4, 4, &endIndex, &trueSum, GetValue));
	JAssertEqual(7, endIndex);
	JAssertEqual(4, trueSum);
	JAssertTrue(a.FindPositiveSum(6, 4, &endIndex, &trueSum, GetValue));
	JAssertEqual(7, endIndex);
	JAssertEqual(4, trueSum);
	JAssertTrue(a.FindPositiveSum(7, 4, &endIndex, &trueSum, GetValue));
	JAssertEqual(8, endIndex);
	JAssertEqual(7, trueSum);
	JAssertFalse(a.FindPositiveSum(20, 12, &endIndex, &trueSum, GetValue));
	JAssertEqual(13, endIndex);
	JAssertEqual(5, trueSum);
	JAssertTrue(a.FindPositiveSum(8, 13, &endIndex, &trueSum, GetValue));
	JAssertEqual(13, endIndex);
	JAssertEqual(0, trueSum);
}

JTEST(FunctionalProgramming)
{
	JRunArray<long> a;
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
	JRunArray<long> a;
	JSetList("5 4 4 2 1", &a);

	JListIterator<long>* iter = a.NewIterator(kJIteratorStartAtEnd);
	JAssertFalse(iter->AtBeginning());
	JAssertTrue(iter->AtEnd());

	JRunArrayIterator<long>* runiter = dynamic_cast<JRunArrayIterator<long>*>(iter);
	JAssertFalse(runiter->AtFirstRun());
	JAssertTrue(runiter->AtLastRun());

	long j;
	JAssertFalse(iter->Next(&j));

	JIndex i;
	JAssertFalse(iter->GetNextElementIndex(&i));
	JAssertTrue(iter->GetPrevElementIndex(&i));
	JAssertEqual(5, i);

	JAssertTrue(iter->Prev(&j));
	JAssertEqual(1, j);
	JAssertEqual(4, iter->GetPrevElementIndex());
	JAssertEqual(5, iter->GetNextElementIndex());
	JAssertFalse(runiter->AtFirstRun());
	JAssertTrue(runiter->AtLastRun());
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(2, j);
	JAssertFalse(runiter->AtFirstRun());
	JAssertFalse(runiter->AtLastRun());
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(4, j);
	JAssertEqual(2, iter->GetPrevElementIndex());
	JAssertEqual(3, iter->GetNextElementIndex());
	JAssertFalse(runiter->AtFirstRun());
	JAssertFalse(runiter->AtLastRun());
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(4, j);
	JAssertFalse(runiter->AtFirstRun());
	JAssertFalse(runiter->AtLastRun());
	JAssertTrue(iter->Prev(&j));
	JAssertEqual(5, j);

	JAssertFalse(iter->Prev(&j));
	JAssertTrue(iter->AtBeginning());
	JAssertFalse(iter->AtEnd());
	JAssertTrue(runiter->AtFirstRun());
	JAssertFalse(runiter->AtLastRun());

	JAssertFalse(iter->GetPrevElementIndex(&i));
	JAssertTrue(iter->GetNextElementIndex(&i));
	JAssertEqual(1, i);

	JAssertTrue(iter->Next(&j));
	JAssertEqual(5, j);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(4, j);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(4, j);
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

	// 5 4 4 2 1

	runiter->MoveTo(kJIteratorStartAtBeginning, 0);
	JAssertEqual(1, runiter->GetRunStart());
	JAssertEqual(1, runiter->GetRunEnd());
	JAssertEqual(1, runiter->GetRunLength());
	JAssertEqual(1, runiter->GetRemainingInRun());
	JAssertEqual(5, runiter->GetRunData());

	JAssertTrue(runiter->NextRun());
	JAssertEqual(2, runiter->GetRunStart());
	JAssertEqual(3, runiter->GetRunEnd());
	JAssertEqual(2, runiter->GetRunLength());
	JAssertEqual(2, runiter->GetRemainingInRun());
	JAssertEqual(4, runiter->GetRunData());
	runiter->SkipNext();
	JAssertEqual(2, runiter->GetRunStart());
	JAssertEqual(3, runiter->GetRunEnd());
	JAssertEqual(2, runiter->GetRunLength());
	JAssertEqual(1, runiter->GetRemainingInRun());
	JAssertEqual(4, runiter->GetRunData());

	JAssertTrue(runiter->NextRun());
	JAssertEqual(4, runiter->GetRunStart());
	JAssertEqual(4, runiter->GetRunEnd());
	JAssertEqual(1, runiter->GetRunLength());
	JAssertEqual(1, runiter->GetRemainingInRun());
	JAssertEqual(2, runiter->GetRunData());

	JAssertTrue(runiter->NextRun());
	JAssertEqual(5, runiter->GetRunStart());
	JAssertEqual(5, runiter->GetRunEnd());
	JAssertEqual(1, runiter->GetRunLength());
	JAssertEqual(1, runiter->GetRemainingInRun());
	JAssertEqual(1, runiter->GetRunData());

	JAssertFalse(runiter->NextRun());

	JAssertTrue(runiter->PrevRun());
	JAssertEqual(5, runiter->GetRunStart());
	JAssertEqual(5, runiter->GetRunEnd());
	JAssertEqual(1, runiter->GetRunLength());
	JAssertEqual(1, runiter->GetRemainingInRun());
	JAssertEqual(1, runiter->GetRunData());

	JAssertTrue(runiter->PrevRun());
	JAssertEqual(4, runiter->GetRunStart());
	JAssertEqual(4, runiter->GetRunEnd());
	JAssertEqual(1, runiter->GetRunLength());
	JAssertEqual(1, runiter->GetRemainingInRun());
	JAssertEqual(2, runiter->GetRunData());

	JAssertTrue(runiter->PrevRun());
	JAssertEqual(2, runiter->GetRunStart());
	JAssertEqual(3, runiter->GetRunEnd());
	JAssertEqual(2, runiter->GetRunLength());
	JAssertEqual(2, runiter->GetRemainingInRun());
	JAssertEqual(4, runiter->GetRunData());

	JAssertFalse(runiter->PrevRun());
	JAssertEqual(1, runiter->GetRunStart());
	JAssertEqual(1, runiter->GetRunEnd());
	JAssertEqual(1, runiter->GetRunLength());
	JAssertEqual(1, runiter->GetRemainingInRun());
	JAssertEqual(5, runiter->GetRunData());

	jdelete iter;
}

JTEST(IteratorConst)
{
	JRunArray<long> a;
	JSetList("5 4 3 2 1", &a);

	const JRunArray<long>& b = a;

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
	JRunArray<long> a;
	JSetList("5 4 4 2 1", &a);

	JListIterator<long>* iter = a.NewIterator();
	JAssertEqual(1, iter->GetNextElementIndex());

	long j;

	a.PrependElement(5);
	JAssertEqual(1, iter->GetNextElementIndex());
	verify("5 5 4 4 2 1", a);

	JAssertTrue(iter->Next(&j));
	JAssertEqual(5, j);
	JAssertEqual(2, iter->GetNextElementIndex());

	a.PrependElement(-2);
	JAssertEqual(3, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(5, j);
	verify("-2 5 5 4 4 2 1", a);

	iter->MoveTo(kJIteratorStartAfter, 4);
	a.AppendElement(-3);
	JAssertEqual(5, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(4, j);
	iter->SkipPrev();
	verify("-2 5 5 4 4 2 1 -3", a);

	{
	JListIterator<long>* i2 = a.NewIterator(kJIteratorStartBefore, 2);
	i2->Insert(-2);
	jdelete i2;
	}

	JAssertEqual(6, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(4, j);
	iter->SkipPrev();
	verify("-2 -2 5 5 4 4 2 1 -3", a);

	iter->SetPrev(-4);
	iter->SetNext(-4);
	JAssertTrue(iter->Next(&j));
	JAssertEqual(-4, j);
	iter->SkipPrev();
	verify("-2 -2 5 5 -4 -4 2 1 -3", a);

	iter->RemovePrev(2);
	iter->RemoveNext(1);
	JAssertEqual(4, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(2, j);
	iter->SkipPrev();
	verify("-2 -2 5 2 1 -3", a);

	iter->Insert(2);
	JAssertEqual(4, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(2, j);
	iter->SkipPrev();
	verify("-2 -2 5 2 2 1 -3", a);

	iter->Insert(5);
	JAssertEqual(4, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(5, j);
	iter->SkipPrev();
	verify("-2 -2 5 5 2 2 1 -3", a);

	jdelete iter;
}

JTEST(TwoIterators)
{
	JRunArray<long> a;
	JSetList("5 5 3 1 1", &a);

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
	verify("5 5 -1 3 1 1", a);

	i1->RemoveNext(2);
	JAssertEqual(3, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(1, j);
	i1->SkipPrev();
	JAssertEqual(4, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("5 5 1 1", a);

	i2->Insert(-2);
	JAssertEqual(3, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(1, j);
	i1->SkipPrev();
	JAssertEqual(4, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("5 5 1 1 -2", a);

	a.PrependElement(5);
	JAssertEqual(4, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(1, j);
	i1->SkipPrev();
	JAssertEqual(5, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("5 5 5 1 1 -2", a);

	{
	JListIterator<long>* i3 = a.NewIterator(kJIteratorStartBefore, 5);
	i3->Insert(1);
	jdelete i3;
	}

	JAssertEqual(4, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(1, j);
	i1->SkipPrev();
	JAssertEqual(6, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("5 5 5 1 1 1 -2", a);

	jdelete i1;
	jdelete i2;
}
