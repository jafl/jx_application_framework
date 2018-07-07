/******************************************************************************
 test_JArray.cpp

	Program to test JArray and JListIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JArray.h>
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
	JArray<long> a1(1); // Ridiculous block size to really exercise resizer
	JBroadcastTester snoop1(&a1);
	JAssertTrue(a1.IsEmpty());
	JAssertEqual(0, a1.GetElementCount());

	long i;
	for (long j : { 1,2,3,4,5 })
		{
		snoop1.Expect(JListT::kElementsInserted,
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

		a1.AppendElement(j);
		}

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(5, a1.GetElementCount());

	snoop1.Expect(JListT::kElementsSwapped,
		[] (const JBroadcaster::Message& m)
		{
			const JListT::ElementsSwapped* es =
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

	a1.SwapElements(2,5);

	snoop1.Expect(JListT::kElementsInserted);
	a1.InsertElementAtIndex(3,1);

	snoop1.Expect(JListT::kElementMoved,
		[] (const JBroadcaster::Message& m)
		{
			const JListT::ElementMoved* em =
				dynamic_cast<const JListT::ElementMoved*>(&m);
			JAssertNotNull(em);
			JAssertEqual(3, em->GetOrigIndex());
			JAssertEqual(6, em->GetNewIndex());

			JIndex k = 2;
			em->AdjustIndex(&k);
			JAssertEqual(2, k);

			k = 3;
			em->AdjustIndex(&k);
			JAssertEqual(6, k);

			k = 4;
			em->AdjustIndex(&k);
			JAssertEqual(3, k);
		});

	a1.MoveElementToIndex(3,a1.GetElementCount());

	JListIterator<long> iter(&a1);

	snoop1.Expect(JListT::kElementsRemoved,
		[] (const JBroadcaster::Message& m)
		{
			const JListT::ElementsRemoved* er =
				dynamic_cast<const JListT::ElementsRemoved*>(&m);
			JAssertNotNull(er);
			JAssertEqual(4, er->GetFirstIndex());
			JAssertEqual(4, er->GetLastIndex());
			JAssertEqual(1, er->GetCount());
			JAssertFalse(er->Contains(3));
			JAssertTrue(er->Contains(4));
			JAssertFalse(er->Contains(5));

			JIndex k = 3;
			er->AdjustIndex(&k);
			JAssertEqual(3, k);

			k = 4;
			er->AdjustIndex(&k);
			JAssertEqual(0, k);

			k = 5;
			er->AdjustIndex(&k);
			JAssertEqual(4, k);
		});

	{
	long expect[] = { 1, 5, 3, 2, 1 };
	long j        = 0;
	while (iter.Next(&i))
		{
		JAssertEqual(expect[j], i);
		j++;

		if (i == 5) a1.RemoveElement(4);
		}
	JAssertEqual(5, j);
	}
	snoop1.Expect(JListT::kElementsRemoved);
	{
	long expect[] = { 1, 2, 3, 5, 1 };
	long j        = 0;
	while (iter.Prev(&i))
		{
		JAssertEqual(expect[j], i);
		j++;

		if (i == 5) a1.RemoveElement(4);
		}
	JAssertEqual(5, j);
	}
	JAssertFalse(a1.IsEmpty());
	JAssertEqual(4, a1.GetElementCount());

	JArray<long> a2 = a1;
{
	JBroadcastTester snoop2(&a2);

	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetElementCount());

	JListIterator<long> iter2(&a2, kJIteratorStartAtEnd);
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
	snoop2.Expect(JListT::kElementsRemoved,
		[] (const JBroadcaster::Message& m)
		{
			const JListT::ElementsRemoved* er =
				dynamic_cast<const JListT::ElementsRemoved*>(&m);
			JAssertNotNull(er);
			JAssertEqual(1, er->GetFirstIndex());
			JAssertEqual(4, er->GetLastIndex());
			JAssertEqual(4, er->GetCount());
			JAssertTrue(er->Contains(3));
			JAssertTrue(er->Contains(4));
			JAssertFalse(er->Contains(5));

			JIndex k = 3;
			er->AdjustIndex(&k);
			JAssertEqual(0, k);
		});

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
	JListIterator<long> iter2(&a2);

// test sort ascending

	a2.SetCompareFunction(CompareLongs);
	a2.SetSortOrder(JListT::kSortAscending);
	a2.Sort();
	{
	long expect[] = { 1, 1, 1, 3, 5 };
	long j        = 0;
	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter2.Next(&i))
		{
		JAssertEqual(expect[j], i);
		j++;
		}
	JAssertEqual(5, j);
	}

// test insertion sort

	{
	JBoolean expect[] = { kJTrue, kJFalse, kJFalse, kJFalse };
	long element[]    = { 3, -1, 10, 4 };
	const long eCount = sizeof(element)/sizeof(long);

	JBoolean isDuplicate;
	for (i=0; i<eCount; i++)
		{
		const JIndex j =
			a2.GetInsertionSortIndex(element[i], &isDuplicate);
		a2.InsertElementAtIndex(j, element[i]);
		JAssertEqual(expect[i], isDuplicate);
		}
	}

// test binary search (sorted ascending)

	{
	long element[]   = {     2,     1,    -1,    10,     -3,     20};
	JBoolean found[] = {kJFalse, kJTrue, kJTrue, kJTrue, kJFalse, kJFalse};
	JIndex first[]   = {     0,     2,     1,     9,      0,      0};
	JIndex last[]    = {     0,     4,     1,     9,      0,      0};
	const long eCount = sizeof(element)/sizeof(long);
	assert( eCount == sizeof(found)/sizeof(JBoolean) );

	for (i=0; i<eCount; i++)
		{
		JIndex j;
		JAssertTrue(
			a2.SearchSorted(element[i], JListT::kFirstMatch, &j) == found[i] &&
			j == first[i] &&
			a2.SearchSorted(element[i], JListT::kLastMatch, &j) == found[i] &&
			j == last[i] &&
			a2.SearchSorted(element[i], JListT::kAnyMatch, &j) == found[i] &&
			first[i] <= j && j <= last[i]);
		}
	}

// test sort descending

	a2.SetSortOrder(JListT::kSortDescending);
	a2.Sort();
	{
	long expect[] = { 10, 5, 4, 3, 3, 1, 1, 1, -1 };
	long j        = 0;
	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter2.Next(&i))
		{
		JAssertEqual(expect[j], i);
		j++;
		}
	JAssertEqual(9, j);
	}

// test binary search (sorted descending)

	{
	long element[]   = {     2,     1,    -1,    10,     -3,     20};
	JBoolean found[] = {kJFalse, kJTrue, kJTrue, kJTrue, kJFalse, kJFalse};
	JIndex first[]   = {     0,     6,     9,     1,      0,      0};
	JIndex last[]    = {     0,     8,     9,     1,      0,      0};
	const long eCount = sizeof(element)/sizeof(long);
	assert( eCount == sizeof(found)/sizeof(JBoolean) );

	for (i=0; i<eCount; i++)
		{
		JIndex j;
		JAssertTrue(
			a2.SearchSorted(element[i], JListT::kFirstMatch, &j) == found[i] &&
			j == first[i] &&
			a2.SearchSorted(element[i], JListT::kLastMatch, &j) == found[i] &&
			j == last[i] &&
			a2.SearchSorted(element[i], JListT::kAnyMatch, &j) == found[i] &&
			first[i] <= j && j <= last[i]);
		}
	}
}

JTEST(SearchSorted1EdgeCases)
{
	const JListT::SortOrder order[] =
		{ JListT::kSortAscending, JListT::kSortDescending };

	const JListT::SearchReturn search[] =
		{ JListT::kFirstMatch, JListT::kAnyMatch, JListT::kLastMatch };

	// one element: 1

	const JBoolean found1[] = { kJFalse, kJTrue, kJFalse };
	const JIndex index1[]   = {      1,     1,      2 };

	// two different elements: 1 3

	const JBoolean found2[] = { kJFalse, kJTrue, kJFalse, kJTrue, kJFalse };
	const JIndex index2[]   = {      1,     1,      2,     2,      3 };

	// two equal elements: 1 1

	const JBoolean found2_same_first[] = { kJFalse, kJTrue, kJFalse };
	const JIndex index2_same_first[]   = {      1,     1,      3 };

	const JBoolean found2_same_any[] = { kJFalse, kJTrue, kJFalse };
	const JIndex index2_same_any[]   = {      1,     1,      3 };

	const JBoolean found2_same_last[] = { kJFalse, kJTrue, kJFalse };
	const JIndex index2_same_last[]   = {      1,     2,      3 };

	const JBoolean* found2_same[] = { found2_same_first, found2_same_any, found2_same_last };
	const JIndex* index2_same[]   = { index2_same_first, index2_same_any, index2_same_last };

	JArray<long> a3(1); // Ridiculous block size to really exercise resizer

	a3.SetCompareFunction(CompareLongs);

	for (const JListT::SortOrder o : order)
		{
		a3.SetSortOrder(o);
		const JBoolean asc = JI2B(o == JListT::kSortAscending);

		long j = 0;
		for (const JListT::SearchReturn s : search)
			{
			int k;
			JBoolean found;

			a3.RemoveAll();
			a3.AppendElement(1);
			for (k=0; k<=2; k++)
				{
				const JIndex index = a3.SearchSorted1(k, s, &found);
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
				const JIndex index = a3.SearchSorted1(k, s, &found);
				JAssertTrue(
					found == found2[asc ? k : 4-k] &&
					index == index2[asc ? k : 4-k] );
				}

			a3.SetElement(1,1);
			a3.SetElement(2,1);
			for (k=0; k<=2; k++)
				{
				const JIndex index = a3.SearchSorted1(k, s, &found);
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

JTEST(Move)
{
	JArray<long>* a1 = jnew JArray<long>;
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
