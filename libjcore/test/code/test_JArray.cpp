/******************************************************************************
 test_JArray.cpp

	Program to test JArray and JArrayIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "jListTestUtil.h"
#include <jx-af/jcore/JArray.h>
#include <jx-af/jcore/JBroadcastTester.h>
#include <algorithm>
#include <numeric>
#include <jx-af/jcore/jAssert.h>

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

int
qsortCompare(const void* e1, const void* e2)
{
	return (*(long*)e1 - *(long*)e2);
}

JTEST(Construct)
{
	JArray<long> a;

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

	const JArray<long> a2 = a;
	verify("1 2 3 4 5", a2);

	JArray<long> a3;
	JAssertTrue(a3.IsEmpty());
	a3 = a;
	verify("1 2 3 4 5", a3);
}

JTEST(Prepend)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JBroadcastTester snoop(&a);

	for (long j : { 5,4,3,2,1 })
	{
		snoop.Expect(JListT::kElementsInserted,
			[] (const JBroadcaster::Message& m)
		{
				const auto* ei =
					dynamic_cast<const JListT::ElementsInserted*>(&m);
				JAssertNotNull(ei);
				JAssertEqual(1, ei->GetFirstIndex());
				JAssertEqual(1, ei->GetLastIndex());
				JAssertEqual(1, ei->GetCount());
		});

		a.PrependElement(j);
	}

	verify("1 2 3 4 5", a);

	JAssertEqual(2, a.GetElement(2));
	JAssertEqual(4, a.GetElementFromEnd(2));
	JAssertEqual(1, a.GetFirstElement());
	JAssertEqual(5, a.GetLastElement());
}

JTEST(Append)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JBroadcastTester snoop(&a);

	for (long j : { 1,2,3,4,5 })
	{
		snoop.Expect(JListT::kElementsInserted,
			[j] (const JBroadcaster::Message& m)
		{
				const auto* ei =
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

	JAssertEqual(2, a.GetElement(2));
	JAssertEqual(4, a.GetElementFromEnd(2));
	JAssertEqual(1, a.GetFirstElement());
	JAssertEqual(5, a.GetLastElement());
}

JTEST(Insert)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JSetList("5 4 3 2 1", &a);
	JBroadcastTester snoop(&a);

	snoop.Expect(JListT::kElementsInserted);
	a.InsertElementAtIndex(1, 6);
	verify("6 5 4 3 2 1", a);

	snoop.Expect(JListT::kElementsInserted);
	a.InsertElementAtIndex(4, -1);
	verify("6 5 4 -1 3 2 1", a);

	snoop.Expect(JListT::kElementsInserted);
	a.InsertElementAtIndex(7, 0);
	verify("6 5 4 -1 3 2 0 1", a);

	snoop.Expect(JListT::kElementsInserted);
	a.InsertElementAtIndex(9, 10);
	verify("6 5 4 -1 3 2 0 1 10", a);
}

JTEST(Remove)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JSetList("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15", &a);
	JBroadcastTester snoop(&a);

	snoop.Expect(JListT::kElementsRemoved,
		[] (const JBroadcaster::Message& m)
	{
			const auto* er =
				dynamic_cast<const JListT::ElementsRemoved*>(&m);
			JAssertNotNull(er);
			JAssertEqual(7, er->GetFirstIndex());
			JAssertEqual(7, er->GetLastIndex());
			JAssertEqual(1, er->GetCount());
			JAssertFalse(er->Contains(6));
			JAssertTrue(er->Contains(7));
			JAssertFalse(er->Contains(8));

			JIndex k = 6;
			er->AdjustIndex(&k);
			JAssertEqual(6, k);

			k = 7;
			er->AdjustIndex(&k);
			JAssertEqual(0, k);

			k = 8;
			er->AdjustIndex(&k);
			JAssertEqual(7, k);
	});
	a.RemoveElement(7);
	verify("1 2 3 4 5 6 8 9 10 11 12 13 14 15", a);

	snoop.Expect(JListT::kElementsRemoved);
	a.RemoveElement(1);
	verify("2 3 4 5 6 8 9 10 11 12 13 14 15", a);

	snoop.Expect(JListT::kElementsRemoved);
	a.RemoveElement(13);
	verify("2 3 4 5 6 8 9 10 11 12 13 14", a);

	snoop.Expect(JListT::kElementsRemoved,
		[] (const JBroadcaster::Message& m)
	{
			const auto* er =
				dynamic_cast<const JListT::ElementsRemoved*>(&m);
			JAssertNotNull(er);
			JAssertEqual(8, er->GetFirstIndex());
			JAssertEqual(10, er->GetLastIndex());
			JAssertEqual(3, er->GetCount());
			JAssertFalse(er->Contains(7));
			JAssertTrue(er->Contains(9));
			JAssertFalse(er->Contains(11));

			JIndex k = 7;
			er->AdjustIndex(&k);
			JAssertEqual(7, k);

			k = 9;
			er->AdjustIndex(&k);
			JAssertEqual(0, k);

			k = 11;
			er->AdjustIndex(&k);
			JAssertEqual(8, k);
	});
	a.RemoveNextElements(8, 3);
	verify("2 3 4 5 6 8 9 13 14", a);

	snoop.Expect(JListT::kElementsRemoved);
	a.RemovePrevElements(5, 2);
	verify("2 3 4 8 9 13 14", a);

	snoop.Expect(JListT::kElementsRemoved);
	a.RemoveAll();
	JAssertTrue(a.IsEmpty());
}

JTEST(Move)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JSetList("1 2 3 4 5", &a);
	JBroadcastTester snoop(&a);

	snoop.Expect(JListT::kElementMoved,
		[] (const JBroadcaster::Message& m)
	{
			const auto* em =
				dynamic_cast<const JListT::ElementMoved*>(&m);
			JAssertNotNull(em);
			JAssertEqual(3, em->GetOrigIndex());
			JAssertEqual(5, em->GetNewIndex());

			JIndex k = 2;
			em->AdjustIndex(&k);
			JAssertEqual(2, k);

			k = 3;
			em->AdjustIndex(&k);
			JAssertEqual(5, k);

			k = 4;
			em->AdjustIndex(&k);
			JAssertEqual(3, k);
	});
	a.MoveElementToIndex(3, a.GetIndexFromEnd(1));
	verify("1 2 4 5 3", a);

	snoop.Expect(JListT::kElementMoved);
	a.MoveElementToIndex(3, 1);
	verify("4 1 2 5 3", a);

	snoop.Expect(JListT::kElementMoved);
	a.MoveElementToIndex(4, 2);
	verify("4 5 1 2 3", a);
}

JTEST(Swap)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JSetList("1 2 3 4 5", &a);
	JBroadcastTester snoop(&a);

	snoop.Expect(JListT::kElementsSwapped,
		[] (const JBroadcaster::Message& m)
	{
			const auto* es =
				dynamic_cast<const JListT::ElementsSwapped*>(&m);
			JAssertNotNull(es);
			JAssertEqual(2, es->GetIndex1());
			JAssertEqual(5, es->GetIndex2());

			JIndex k = 2;
			es->AdjustIndex(&k);
			JAssertEqual(5, k);

			k = 3;
			es->AdjustIndex(&k);
			JAssertEqual(3, k);

			k = 5;
			es->AdjustIndex(&k);
			JAssertEqual(2, k);
	});

	a.SwapElements(2,5);
	verify("1 5 3 4 2", a);
}

JTEST(Sort)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JSetList("3 1 5 1 1", &a);

// test sort ascending

	a.SetCompareFunction(CompareLongs);
	a.SetSortOrder(JListT::kSortAscending);
	a.Sort();
	verify("1 1 1 3 5", a);
	JAssertTrue(a.IsSorted());

// test insertion sort

{
	bool expect[] = { true, false, false, false };
	long element[]    = { 3, -1, 10 };
	const long eCount = sizeof(element)/sizeof(long);

	bool isDuplicate;
	for (JIndex i=0; i<eCount; i++)
	{
		const JIndex j = a.GetInsertionSortIndex(element[i], &isDuplicate);
		a.InsertElementAtIndex(j, element[i]);
		JAssertEqual(expect[i], (bool) isDuplicate);
	}
}
	verify("-1 1 1 1 3 3 5 10", a);

	JAssertTrue(a.InsertSorted(4));
	JAssertFalse(a.InsertSorted(3, false));
	verify("-1 1 1 1 3 3 4 5 10", a);

// test binary search (sorted ascending)

{
	long element[]   = {     2,     1,    -1,    10,     -3,     20};
	bool found[] = {false, true, true, true, false, false};
	JIndex first[]   = {     0,     2,     1,     9,      0,      0};
	JIndex last[]    = {     0,     4,     1,     9,      0,      0};
	const long eCount = sizeof(element)/sizeof(long);
	assert( eCount == sizeof(found)/sizeof(bool) );

	for (JIndex i=0; i<eCount; i++)
	{
		JIndex j;
		JAssertTrue(
			a.SearchSorted(element[i], JListT::kFirstMatch, &j) == found[i] &&
			j == first[i] &&
			a.SearchSorted(element[i], JListT::kLastMatch, &j) == found[i] &&
			j == last[i] &&
			a.SearchSorted(element[i], JListT::kAnyMatch, &j) == found[i] &&
			first[i] <= j && j <= last[i]);
	}
}

// test sort descending

	a.SetSortOrder(JListT::kSortDescending);
	a.Sort();
	verify("10 5 4 3 3 1 1 1 -1", a);

// test binary search (sorted descending)

{
	long element[]   = {     2,     1,    -1,    10,     -3,     20};
	bool found[] = {false, true, true, true, false, false};
	JIndex first[]   = {     0,     6,     9,     1,      0,      0};
	JIndex last[]    = {     0,     8,     9,     1,      0,      0};
	const long eCount = sizeof(element)/sizeof(long);
	assert( eCount == sizeof(found)/sizeof(bool) );

	for (JIndex i=0; i<eCount; i++)
	{
		JIndex j;
		JAssertTrue(
			a.SearchSorted(element[i], JListT::kFirstMatch, &j) == found[i] &&
			j == first[i] &&
			a.SearchSorted(element[i], JListT::kLastMatch, &j) == found[i] &&
			j == last[i] &&
			a.SearchSorted(element[i], JListT::kAnyMatch, &j) == found[i] &&
			first[i] <= j && j <= last[i]);
	}
}

// quick sort

	a.QuickSort(qsortCompare);
	verify("-1 1 1 1 3 3 4 5 10", a);
}

JTEST(SearchSortedOTIEdgeCases)
{
	const JListT::SortOrder order[] =
	{ JListT::kSortAscending, JListT::kSortDescending };

	const JListT::SearchReturn search[] =
	{ JListT::kFirstMatch, JListT::kAnyMatch, JListT::kLastMatch };

	// one element: 1

	const bool found1[] = { false, true, false };
	const JIndex index1[]   = {      1,     1,      2 };

	// two different elements: 1 3

	const bool found2[] = { false, true, false, true, false };
	const JIndex index2[]   = {      1,     1,      2,     2,      3 };

	// two equal elements: 1 1

	const bool found2_same_first[] = { false, true, false };
	const JIndex index2_same_first[]   = {      1,     1,      3 };

	const bool found2_same_any[] = { false, true, false };
	const JIndex index2_same_any[]   = {      1,     1,      3 };

	const bool found2_same_last[] = { false, true, false };
	const JIndex index2_same_last[]   = {      1,     2,      3 };

	const bool* found2_same[] = { found2_same_first, found2_same_any, found2_same_last };
	const JIndex* index2_same[]   = { index2_same_first, index2_same_any, index2_same_last };

	JArray<long> a3(1); // Ridiculous block size to really exercise resizer

	a3.SetCompareFunction(CompareLongs);

	for (const JListT::SortOrder o : order)
	{
		a3.SetSortOrder(o);
		const bool asc = o == JListT::kSortAscending;

		long j = 0;
		for (const JListT::SearchReturn s : search)
		{
			int k;
			bool found;

			a3.RemoveAll();
			a3.AppendElement(1);
			for (k=0; k<=2; k++)
			{
				const JIndex index = a3.SearchSortedOTI(k, s, &found);
				JAssertTrue(
					found == found1[asc ? k : 2-k] &&
					index == index1[asc ? k : 2-k] );
			}

			if (asc)
			{
				a3.AppendElement(3);
			}
			else
			{
				a3.PrependElement(3);
			}
			for (k=0; k<=4; k++)
			{
				const JIndex index = a3.SearchSortedOTI(k, s, &found);
				JAssertTrue(
					found == found2[asc ? k : 4-k] &&
					index == index2[asc ? k : 4-k] );
			}

			a3.SetElement(1,1);
			a3.SetElement(2,1);
			for (k=0; k<=2; k++)
			{
				const JIndex index = a3.SearchSortedOTI(k, s, &found);
				JAssertTrue(
					found == found2_same[j][asc ? k : 2-k] &&
					index == index2_same[j][asc ? k : 2-k] );
			}

			j++;
		}
	}
}

JTEST(RangeBasedForLoop)
{
	JArray<long> a;
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
	JArray<long> a;
	a.AppendElement(3);
	a.AppendElement(2);
	a.AppendElement(5);

	long sum = std::accumulate(begin(a), end(a), 0);
	JAssertEqual(10, sum);

	sum = 0;
	std::for_each(begin(a), end(a), [&sum](long v){ sum += v; });
	JAssertEqual(10, sum);
}

JTEST(MoveCtor)
{
	auto* a1 = jnew JArray<long>;
	assert( a1 != nullptr );
	a1->AppendElement(3);
	a1->AppendElement(2);
	a1->AppendElement(5);

	JArray<long> a2(std::move(*a1));

	jdelete a1;
	a1 = nullptr;

	JAssertEqual(3, a2.GetElementCount());
	JAssertEqual(5, a2.GetElement(3));
}

bool matchGreater(const long v)
{
	return v > 3;
};

bool matchLess(const long v)
{
	return v < 3;
};

JTEST(Iterator)
{
	JArray<long> a;
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

	iter->MoveTo(kJIteratorStartAtBeginning, 0);
	JAssertTrue(iter->Next(matchGreater, &j));
	JAssertEqual(5, j);
	JAssertTrue(iter->Next(matchGreater, &j));
	JAssertEqual(4, j);
	JAssertTrue(iter->Next(matchLess, &j));
	JAssertEqual(2, j);
	JAssertFalse(iter->Next(matchGreater, &j));
	JAssertTrue(iter->AtEnd());

	JAssertTrue(iter->Prev(matchLess, &j));
	JAssertEqual(1, j);
	JAssertTrue(iter->Prev(matchLess, &j));
	JAssertEqual(2, j);
	JAssertTrue(iter->Prev(matchGreater, &j));
	JAssertEqual(4, j);
	JAssertFalse(iter->Prev(matchLess, &j));
	JAssertTrue(iter->AtBeginning());

	jdelete iter;
}

JTEST(IteratorConst)
{
	JArray<long> a;
	JSetList("5 4 3 2 1", &a);

	const JArray<long>& b = a;

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
	JArray<long> a;
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

	a.InsertElementAtIndex(2, -4);
	JAssertEqual(6, iter->GetNextElementIndex());
	JAssertTrue(iter->Next(&j));
	JAssertEqual(3, j);
	iter->SkipPrev();
	verify("-2 -4 -1 5 4 3 2 1 -3", a);

	iter->SetPrev(-4, kJIteratorStay);
	iter->SetNext(-3, kJIteratorStay);
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
	JArray<long> a;
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

	a.SwapElements(1, 6);
	JAssertEqual(4, i1->GetNextElementIndex());
	JAssertTrue(i1->Next(&j));
	JAssertEqual(2, j);
	i1->SkipPrev();
	JAssertEqual(5, i2->GetPrevElementIndex());
	JAssertTrue(i2->Prev(&j));
	JAssertEqual(1, j);
	i2->SkipNext();
	verify("-2 5 4 2 1 -3", a);

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
	verify("-2 5 4 -4 2 1 -3", a);

	jdelete i1;
	jdelete i2;
}
