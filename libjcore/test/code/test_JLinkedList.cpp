/******************************************************************************
 test_JLinkedList.cc

	Program to test JLinkedList and JLinkedListIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JLinkedList.h>
#include <JBroadcastTester.h>
#include <jassert_simple.h>

int main()
{
	return JUnitTestManager::Execute();
}

JOrderedSetT::CompareResult
CompareLongs
	(
	const long& a,
	const long& b
	)
{
	if (a < b)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (a == b)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

JTEST(Exercise)
{
	JLinkedList<long> a1;
	JBroadcastTester snoop1(&a1);
	JAssertTrue(a1.IsEmpty());
	JAssertEqual(0, a1.GetElementCount());

	long i;
	for (i=1;i<=5;i++)
		{
		snoop1.Expect(JOrderedSetT::kElementsInserted);
		a1.AppendElement(i);
		}

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(5, a1.GetElementCount());

	snoop1.Expect(JOrderedSetT::kElementsSwapped);
	a1.SwapElements(2,5);

	snoop1.Expect(JOrderedSetT::kElementsInserted);
	a1.InsertElementAtIndex(3,1);

	snoop1.Expect(JOrderedSetT::kElementMoved);
	a1.MoveElementToIndex(3,a1.GetElementCount());

	JOrderedSetIterator<long> iter(&a1);

	snoop1.Expect(JOrderedSetT::kElementsRemoved);
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
	snoop1.Expect(JOrderedSetT::kElementsRemoved);
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

	JLinkedList<long> a2 = a1;
{
	JBroadcastTester snoop2(&a2);

	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetElementCount());

	JOrderedSetIterator<long> iter2(&a2, kJIteratorStartAtEnd);
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
	snoop2.Expect(JOrderedSetT::kElementsRemoved);

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
	snoop2.Expect(JOrderedSetT::kElementsInserted);
	a2.AppendElement(1);
}
	JOrderedSetIterator<long> iter2(&a2);

// test sort ascending

	a2.SetCompareFunction(CompareLongs);
	a2.SetSortOrder(JOrderedSetT::kSortAscending);
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
	long k = 0;
	for (i=0; i<eCount; i++)
		{
		const JIndex j =
			a2.GetInsertionSortIndex(element[i], &isDuplicate);
		a2.InsertElementAtIndex(j, element[i]);
		JAssertEqual(expect[k], isDuplicate);
		k++;
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
			a2.SearchSorted(element[i], JOrderedSetT::kFirstMatch, &j) == found[i] &&
			j == first[i] &&
			a2.SearchSorted(element[i], JOrderedSetT::kLastMatch, &j) == found[i] &&
			j == last[i] &&
			a2.SearchSorted(element[i], JOrderedSetT::kAnyMatch, &j) == found[i] &&
			first[i] <= j && j <= last[i]);
		}
	}

// test sort descending

	a2.SetSortOrder(JOrderedSetT::kSortDescending);
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
			a2.SearchSorted(element[i], JOrderedSetT::kFirstMatch, &j) == found[i] &&
			j == first[i] &&
			a2.SearchSorted(element[i], JOrderedSetT::kLastMatch, &j) == found[i] &&
			j == last[i] &&
			a2.SearchSorted(element[i], JOrderedSetT::kAnyMatch, &j) == found[i] &&
			first[i] <= j && j <= last[i]);
		}
	}
}

JTEST(SearchSorted1EdgeCases)
{
	const JOrderedSetT::SortOrder order[] =
		{ JOrderedSetT::kSortAscending, JOrderedSetT::kSortDescending };

	const JOrderedSetT::SearchReturn search[] =
		{ JOrderedSetT::kFirstMatch, JOrderedSetT::kAnyMatch, JOrderedSetT::kLastMatch };

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

	JLinkedList<long> a3;

	a3.SetCompareFunction(CompareLongs);

	for (int i=0; i<2; i++)
		{
		a3.SetSortOrder(order[i]);

		for (int j=0; j<3; j++)
			{
			int k;
			JBoolean found;

			a3.RemoveAll();
			a3.AppendElement(1);
			for (k=0; k<=2; k++)
				{
				const JIndex index = a3.SearchSorted1(k, search[j], &found);
				JAssertTrue(
					found == found1[i==0 ? k : 2-k] &&
					index == index1[i==0 ? k : 2-k] );
				}

			if (order[i] == JOrderedSetT::kSortAscending)
				{
				a3.AppendElement(3);
				}
			else
				{
				assert( order[i] == JOrderedSetT::kSortDescending );
				a3.PrependElement(3);
				}
			for (k=0; k<=4; k++)
				{
				const JIndex index = a3.SearchSorted1(k, search[j], &found);
				JAssertTrue(
					found == found2[i==0 ? k : 4-k] &&
					index == index2[i==0 ? k : 4-k] );
				}

			a3.SetElement(1,1);
			a3.SetElement(2,1);
			for (k=0; k<=2; k++)
				{
				const JIndex index = a3.SearchSorted1(k, search[j], &found);
				JAssertTrue(
					found == found2_same[j][i==0 ? k : 2-k] &&
					index == index2_same[j][i==0 ? k : 2-k] );
				}
			}
		}
}
