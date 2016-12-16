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

	std::cout << "linked list a1 created" << std::endl << std::endl;

JBroadcastSnooper snoop1(&a1);

	std::cout << "a1 address: " << (void*) &a1 << std::endl;

	std::cout << "a1 itemCount should be 0" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	long i;
	for (i=1;i<=5;i++)
		{
		a1.AppendElement(i);
		}

	std::cout << "a1 itemCount should be 5" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	JWaitForReturn();

	a1.MoveElementToIndex(1,3);
	a1.MoveElementToIndex(3,1);

	a1.SwapElements(2,5);

	a1.InsertElementAtIndex(3,1);

	a1.MoveElementToIndex(3,a1.GetElementCount());

JLinkedListIterator<long> iter(&a1, kJIteratorStartAtBeginning);

	std::cout << "display should be:  1 5 3 2 1" << std::endl;

	while (iter.Next(&i))
		{
		std::cout << i << ' ';

		if (i == 5) a1.RemoveElement(4);
		}
	std::cout << std::endl;

	std::cout << "display should be:  1 2 3 5 1" << std::endl;

	while (iter.Prev(&i))
		{
		std::cout << i << ' ';

		if (i == 5) a1.RemoveElement(4);
		}
	std::cout << std::endl;

	std::cout << "a1 itemCount should be 4" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	JWaitForReturn();

JLinkedList<long> a2 = a1;										// copy constructor

	std::cout << "linked list a2 created from a1" << std::endl << std::endl;

JBroadcastSnooper snoop2(&a2);

	std::cout << "a2 address: " << (void*) &a2 << std::endl;

	std::cout << "a2 itemCount should be 4" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

JLinkedListIterator<long> iter2(&a2,kJIteratorStartAtEnd);

	std::cout << "display should be: 1 3 5 1" << std::endl;

	while (iter2.Prev(&i))
		{
		std::cout << i << ' ';
		}
	std::cout << std::endl;

	a2.RemoveAll();

	std::cout << "a2 itemCount should be 0" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	a2 = a1;													// assignment operator

	std::cout << "linked list a2 assigned from a1" << std::endl << std::endl;

	std::cout << "a2 itemCount should be 4" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	std::cout << "display should be: 1 3 5 1" << std::endl;

	iter2.MoveTo(kJIteratorStartAtEnd, 0);

	while (iter2.Prev(&i))
		{
		std::cout << i << ' ';
		}
	std::cout << std::endl;

	JWaitForReturn();

// test sort ascending

	a2.AppendElement(1);

	a2.SetCompareFunction(CompareLongs);
	a2.SetSortOrder(JOrderedSetT::kSortAscending);
	a2.Sort();

	std::cout << "display should be:" << std::endl;
	std::cout << "1 1 1 3 5" << std::endl;

	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter2.Next(&i))
		{
		std::cout << i << ' ';
		}
	std::cout << std::endl << std::endl;

// test insertion sort

	std::cout << "display should be: T F F F" << std::endl;
	{
	long element[] = {3, -1, 10, 4};
	const long eCount = sizeof(element)/sizeof(long);

	JBoolean isDuplicate;
	for (i=0; i<eCount; i++)
		{
		const JIndex j =
			a2.GetInsertionSortIndex(element[i], &isDuplicate);
		a2.InsertElementAtIndex(j, element[i]);
		std::cout << isDuplicate << ' ';
		}
	std::cout << std::endl << std::endl;
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
			std::cout << i+1 << ": correct" << std::endl;
			}
		else
			{
			std::cout << i+1 << ": WRONG" << std::endl;
			}
		}
	}

	JWaitForReturn();

// test sort descending

	a2.SetSortOrder(JOrderedSetT::kSortDescending);
	a2.Sort();

	std::cout << "display should be:" << std::endl;
	std::cout << "10 5 4 3 3 1 1 1 -1" << std::endl;

	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter2.Next(&i))
		{
		std::cout << i << ' ';
		}
	std::cout << std::endl << std::endl;

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
			std::cout << i+1 << ": correct" << std::endl;
			}
		else
			{
			std::cout << i+1 << ": WRONG" << std::endl;
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
