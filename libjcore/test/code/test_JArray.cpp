/******************************************************************************
 test_JArray.cc

	Program to test JArray and JOrderedSetIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JArray.h>
#include <JBroadcastSnooper.h>
#include <jCommandLine.h>
#include <jAssert.h>

static JOrderedSetT::CompareResult
	CompareLongs(const long& a, const long& b);

int main()
{
JArray<long> a1;												// constructor

	cout << "array a1 created" << endl << endl;

JBroadcastSnooper snoop1(&a1);

	cout << "a1 address: " << (void*) &a1 << endl;

	cout << "a1 itemCount should be 0" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	long i;
	for (i=1;i<=5;i++)
		{
		a1.AppendElement(i);
		}

	cout << "a1 itemCount should be 5" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl;

	JWaitForReturn();

	a1.SwapElements(2,5);

	a1.InsertElementAtIndex(3,1);

	a1.MoveElementToIndex(3,a1.GetElementCount());

JOrderedSetIterator<long> iter(&a1);

	cout << "display should be:  1 5 3 2 1" << endl;

	while (iter.Next(&i))
		{
		cout << i << ' ';

		if (i == 5) a1.RemoveElement(4);
		}
	cout << endl;

	cout << "display should be:  1 2 3 5 1" << endl;

	while (iter.Prev(&i))
		{
		cout << i << ' ';

		if (i == 5) a1.RemoveElement(4);
		}
	cout << endl;

	cout << "a1 itemCount should be 4" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl;

	JWaitForReturn();

JArray<long> a2 = a1;											// copy constructor

	cout << "array a2 created from a1" << endl << endl;

JBroadcastSnooper snoop2(&a2);

	cout << "a2 address: " << (void*) &a2 << endl;

	cout << "a2 itemCount should be 4" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

JOrderedSetIterator<long> iter2(&a2, kJIteratorStartAtEnd);

	cout << "display should be: 1 3 5 1" << endl;

	while (iter2.Prev(&i))
		{
		cout << i << ' ';
		}
	cout << endl;

	a2.RemoveAll();

	cout << "a2 itemCount should be 0" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	a2 = a1;													// assignment operator

	cout << "array a2 assigned from a1" << endl << endl;

	cout << "a2 itemCount should be 4" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	cout << "display should be: 1 3 5 1" << endl;

	iter2.MoveTo(kJIteratorStartAtEnd, 0);

	while (iter2.Prev(&i))
		{
		cout << i << ' ';
		}
	cout << endl;

	JWaitForReturn();

// test sort ascending

	a2.AppendElement(1);

	a2.SetCompareFunction(CompareLongs);
	a2.SetSortOrder(JOrderedSetT::kSortAscending);
	a2.Sort();

	cout << "display should be:" << endl;
	cout << "1 1 1 3 5" << endl;

	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter2.Next(&i))
		{
		cout << i << ' ';
		}
	cout << endl << endl;

// test insertion sort

	cout << "display should be: T F F F" << endl;
	{
	long element[] = {3, -1, 10, 4};
	const long eCount = sizeof(element)/sizeof(long);

	JBoolean isDuplicate;
	for (i=0; i<eCount; i++)
		{
		const JIndex j =
			a2.GetInsertionSortIndex(element[i], &isDuplicate);
		a2.InsertElementAtIndex(j, element[i]);
		cout << isDuplicate << ' ';
		}
	cout << endl << endl;
	}

// test binary search (sorted ascending)
// -1 1 1 1 3 3 4 5 10
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
		if (a2.SearchSorted(element[i], JOrderedSetT::kFirstMatch, &j) == found[i] &&
			j == first[i] &&
			a2.SearchSorted(element[i], JOrderedSetT::kLastMatch, &j) == found[i] &&
			j == last[i] &&
			a2.SearchSorted(element[i], JOrderedSetT::kAnyMatch, &j) == found[i] &&
			first[i] <= j && j <= last[i])
			{
			cout << i+1 << ": correct" << endl;
			}
		else
			{
			cout << i+1 << ": WRONG" << endl;
			}
		}
	}

	JWaitForReturn();

// test sort descending

	a2.SetSortOrder(JOrderedSetT::kSortDescending);
	a2.Sort();

	cout << "display should be:" << endl;
	cout << "10 5 4 3 3 1 1 1 -1" << endl;

	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter2.Next(&i))
		{
		cout << i << ' ';
		}
	cout << endl << endl;

// test binary search (sorted descending)
// 10 5 4 3 3 1 1 1 -1
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
		if (a2.SearchSorted(element[i], JOrderedSetT::kFirstMatch, &j) == found[i] &&
			j == first[i] &&
			a2.SearchSorted(element[i], JOrderedSetT::kLastMatch, &j) == found[i] &&
			j == last[i] &&
			a2.SearchSorted(element[i], JOrderedSetT::kAnyMatch, &j) == found[i] &&
			first[i] <= j && j <= last[i])
			{
			cout << i+1 << ": correct" << endl;
			}
		else
			{
			cout << i+1 << ": WRONG" << endl;
			}
		}
	}

	JWaitForReturn();

// test edge cases of SearchSorted1()

	cout << "Testing SearchSorted1(): no asserts means everything worked" << endl;

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

JArray<long> a3;

	a3.SetCompareFunction(CompareLongs);

	for (i=0; i<2; i++)
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
				assert( found == found1[i==0 ? k : 2-k] &&
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
				assert( found == found2[i==0 ? k : 4-k] &&
						index == index2[i==0 ? k : 4-k] );
				}

			a3.SetElement(1,1);
			a3.SetElement(2,1);
			for (k=0; k<=2; k++)
				{
				const JIndex index = a3.SearchSorted1(k, search[j], &found);
				assert( found == found2_same[j][i==0 ? k : 2-k] &&
						index == index2_same[j][i==0 ? k : 2-k] );
				}
			}
		}
	}

	return 0;
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
