/******************************************************************************
 test_JLinkedList.cc

	Program to test JLinkedList and JLinkedListIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JLinkedList.h>
#include <JBroadcastSnooper.h>
#include <jCommandLine.h>
#include <jAssert.h>

static JOrderedSetT::CompareResult
	CompareLongs(const long& a, const long& b);

int main()
{
JLinkedList<long> a1;											// constructor

	cout << "linked list a1 created" << endl << endl;

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
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	JWaitForReturn();

	a1.MoveElementToIndex(1,3);
	a1.MoveElementToIndex(3,1);

	a1.SwapElements(2,5);

	a1.InsertElementAtIndex(3,1);

	a1.MoveElementToIndex(3,a1.GetElementCount());

JLinkedListIterator<long> iter(&a1, kJIteratorStartAtBeginning);

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
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	JWaitForReturn();

JLinkedList<long> a2 = a1;										// copy constructor

	cout << "linked list a2 created from a1" << endl << endl;

JBroadcastSnooper snoop2(&a2);

	cout << "a2 address: " << (void*) &a2 << endl;

	cout << "a2 itemCount should be 4" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

JLinkedListIterator<long> iter2(&a2,kJIteratorStartAtEnd);

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

	cout << "linked list a2 assigned from a1" << endl << endl;

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
