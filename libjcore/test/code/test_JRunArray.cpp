/******************************************************************************
 test_JRunArray.cc

	Program to test JRunArray and JRunArrayIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JRunArray.h>
#include <JString.h>
#include <JBroadcastSnooper.h>
#include <jCommandLine.h>
#include <sstream>
#include <jAssert.h>

#define verify2(str)	VerifyContents(iter2, str, __LINE__)

void PrintArray(ostream& output, JRunArrayIterator<long>& iter);
void VerifyContents(JRunArrayIterator<long>& iter, const JCharacter* outputStr, const int line);

static JOrderedSetT::CompareResult
	CompareLongs(const long& a, const long& b);

JInteger GetValue(const long& data);

static int QuickSortCompareLongs(const void* a, const void* b);

int main()
{
JRunArray<long> a1;												// constructor

	cout << "array a1 created" << endl << endl;

JBroadcastSnooper snoop1(&a1);

	cout << "a1 address: " << (void*) &a1 << endl;

	cout << "a1 itemCount should be 0" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	long i,j;

	for (i=1;i<=5;i++)
		{
		a1.AppendElement(i);
		}

	// 1 2 3 4 5

	cout << "a1 itemCount should be 5" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	JWaitForReturn();

	a1.SetElement(5,1);
	// 1 2 3 4 1
	a1.SwapElements(2,5);
	// 1 1 3 4 2

	a1.InsertElementAtIndex(3,1);
	// 1 1 1 3 4 2

	a1.SetElement(2,2);
	// 1 2 1 3 4 2
	a1.SetElement(2,1);
	// 1 1 1 3 4 2

	a1.MoveElementToIndex(3,a1.GetElementCount());
	// 1 1 3 4 2 1

JRunArrayIterator<long> iter(&a1,kJIteratorStartAtBeginning);

	cout << "display should be:  1 3 3 2 1" << endl;

	j = 0;
	while (iter.Next(&i))
		{
		j++;
		cout << i << ' ';

		if (j == 1)
			{
			a1.SetElement(2,3);
			// 1 3 3 4 2 1
			}
		else if (j == 3)
			{
			a1.RemoveElement(4);
			// 1 3 3 2 1
			}
		}
	cout << endl;

	cout << "display should be:  1 2 3 1 1" << endl;

	j = 0;
	while (iter.Prev(&i))
		{
		j++;
		cout << i << ' ';

		if (j == 3)
			{
			a1.SetElement(2,1);
			// 1 1 3 2 1
			a1.RemoveElement(4);
			// 1 1 3 1
			}
		}
	cout << endl;

	cout << "a1 itemCount should be 4" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	JWaitForReturn();

JRunArray<long> a2 = a1;										// copy constructor

	cout << "array a2 created from a1" << endl << endl;

JBroadcastSnooper snoop2(&a2);

	cout << "a2 address: " << (void*) &a2 << endl;

	cout << "a2 itemCount should be 4" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

JRunArrayIterator<long> iter2(&a2,kJIteratorStartAtEnd);

	cout << "display should be: 1 3 1 1" << endl;

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

	cout << "display should be: 1 3 1 1" << endl;

	iter2.MoveTo(kJIteratorStartAtEnd, 0);
	while (iter2.Prev(&i))
		{
		cout << i << ' ';
		}
	cout << endl;

	JWaitForReturn();

// test sort ascending

	a2.AppendElement(3);
	a2.AppendElement(5);
	a2.AppendElement(2);
	a2.AppendElement(1);
	a2.AppendElement(5);

	a2.SetCompareFunction(CompareLongs);
	a2.SetSortOrder(JOrderedSetT::kSortAscending);
	a2.Sort();

	cout << "display should be:" << endl;
	cout << "1 1 1 1 2 3 3 5 5" << endl;

	PrintArray(cout, iter2);
	cout << endl;

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
// -1 1 1 1 1 2 3 3 3 4 5 5 10
	{
	long element[]   = {     0,     4,    -1,    10,     -3,     20};
	JBoolean found[] = {kJFalse, kJTrue, kJTrue, kJTrue, kJFalse, kJFalse};
	JIndex pos[]     = {     0,     10,    1,    13,      0,      0};
	const long eCount = sizeof(element)/sizeof(long);
	assert( eCount == sizeof(found)/sizeof(JBoolean) );

	for (i=0; i<eCount; i++)
		{
		JIndex p1=0, p2=0, p3=0;
		if (a2.SearchSorted(element[i], JOrderedSetT::kFirstMatch, &p1) == found[i] &&
			pos[i] == p1 &&
			a2.SearchSorted(element[i], JOrderedSetT::kLastMatch, &p2) == found[i] &&
			pos[i] == p2 &&
			a2.SearchSorted(element[i], JOrderedSetT::kAnyMatch, &p3) == found[i] &&
			pos[i] == p3)
			{
			cout << i+1 << ": correct" << endl;
			}
		else
			{
			cout << i+1 << ": WRONG" << endl;
			}
		}

	JIndex p4=0, p5=0;
	if (a2.SearchSorted(1, JOrderedSetT::kFirstMatch, &p4) &&
		a2.SearchSorted(1, JOrderedSetT::kLastMatch, &p5) &&
		p4 == 2 && p5 == 5)
		{
		cout << eCount+1 << ": correct" << endl;
		}
	else
		{
		cout << eCount+1 << ": WRONG" << endl;
		}
	}

	JWaitForReturn();

// test sort descending

	a2.SetSortOrder(JOrderedSetT::kSortDescending);
	a2.Sort();

	cout << "display should be:" << endl;
	cout << "10 5 5 4 3 3 3 2 1 1 1 1 -1" << endl;

	PrintArray(cout, iter2);
	cout << endl;

// test binary search (sorted descending)
// 10 5 5 4 3 3 3 2 1 1 1 1 -1
	{
	long element[]   = {     0,     4,    -1,    10,     -3,     20};
	JBoolean found[] = {kJFalse, kJTrue, kJTrue, kJTrue, kJFalse, kJFalse};
	JIndex pos[]     = {     0,     4,    13,    1,       0,      0};
	const long eCount = sizeof(element)/sizeof(long);
	assert( eCount == sizeof(found)/sizeof(JBoolean) );

	for (i=0; i<eCount; i++)
		{
		JIndex p6=0, p7=0, p8=0;
		if (a2.SearchSorted(element[i], JOrderedSetT::kFirstMatch, &p6) == found[i] &&
			pos[i] == p6 &&
			a2.SearchSorted(element[i], JOrderedSetT::kLastMatch, &p7) == found[i] &&
			pos[i] == p7 &&
			a2.SearchSorted(element[i], JOrderedSetT::kAnyMatch, &p8) == found[i] &&
			pos[i] == p8)
			{
			cout << i+1 << ": correct" << endl;
			}
		else
			{
			cout << i+1 << ": WRONG" << endl;
			}
		}

	JIndex p9=0, p10=0;
	if (a2.SearchSorted(3, JOrderedSetT::kFirstMatch, &p9) &&
		a2.SearchSorted(3, JOrderedSetT::kLastMatch, &p10) &&
		p9 == 5 && p10 == 7)
		{
		cout << eCount+1 << ": correct" << endl;
		}
	else
		{
		cout << eCount+1 << ": WRONG" << endl;
		}
	}

	JWaitForReturn();

	a2.QuickSort(QuickSortCompareLongs);

	cout << "display should be:" << endl;
	cout << "-1 1 1 1 1 2 3 3 3 4 5 5 10" << endl;

	PrintArray(cout, iter2);
	JWaitForReturn();

// test SumElements()

	JInteger sum = a2.SumElements(1,1, GetValue);
	assert( sum == -1 );
	sum = a2.SumElements(1,2, GetValue);
	assert( sum == 0 );
	sum = a2.SumElements(1,5, GetValue);
	assert( sum == 3 );
	sum = a2.SumElements(1,8, GetValue);
	assert( sum == 11 );
	sum = a2.SumElements(2,8, GetValue);
	assert( sum == 12 );
	sum = a2.SumElements(4,8, GetValue);
	assert( sum == 10 );
	sum = a2.SumElements(4,9, GetValue);
	assert( sum == 13 );
	sum = a2.SumElements(6,6, GetValue);
	assert( sum == 2 );

	JIndex endIndex;
	JInteger trueSum;
	JBoolean foundSum = a2.FindPositiveSum(1, 1, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 4 && trueSum == 1 );
	foundSum = a2.FindPositiveSum(3, 1, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 6 && trueSum == 3 );
	foundSum = a2.FindPositiveSum(11, 1, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 9 && trueSum == 11 );
	foundSum = a2.FindPositiveSum(10, 1, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 8 && trueSum == 8 );
	foundSum = a2.FindPositiveSum(9, 2, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 8 && trueSum == 9 );
	foundSum = a2.FindPositiveSum(10, 2, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 8 && trueSum == 9 );
	foundSum = a2.FindPositiveSum(15, 2, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 10 && trueSum == 15 );
	foundSum = a2.FindPositiveSum(4, 4, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 7 && trueSum == 4 );
	foundSum = a2.FindPositiveSum(6, 4, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 7 && trueSum == 4 );
	foundSum = a2.FindPositiveSum(7, 4, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 8 && trueSum == 7 );
	foundSum = a2.FindPositiveSum(20, 12, &endIndex, &trueSum, GetValue);
	assert( !foundSum && endIndex == 13 && trueSum == 5 );
	foundSum = a2.FindPositiveSum(8, 13, &endIndex, &trueSum, GetValue);
	assert( foundSum && endIndex == 13 && trueSum == 0 );

// test optimizations

	a2.InsertElementsAtIndex(3, 1, 2);
	verify2("-1 1 1 1 1 1 1 2 3 3 3 4 5 5 10");
	a2.InsertElementsAtIndex(2, -1, 2);
	verify2("-1 -1 -1 1 1 1 1 1 1 2 3 3 3 4 5 5 10");
	a2.InsertElementsAtIndex(4, 0, 2);
	verify2("-1 -1 -1 0 0 1 1 1 1 1 1 2 3 3 3 4 5 5 10");
	a2.InsertElementsAtIndex(11, 2, 2);
	verify2("-1 -1 -1 0 0 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10");
	a2.AppendElements(10, 2);
	verify2("-1 -1 -1 0 0 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10 10 10");
	a2.AppendElements(11, 2);
	verify2("-1 -1 -1 0 0 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10 10 10 11 11");

	a2.SetElement(4, -1);
	verify2("-1 -1 -1 -1 0 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10 10 10 11 11");
	a2.SetElement(5, -1);
	verify2("-1 -1 -1 -1 -1 1 1 1 1 1 2 2 1 2 3 3 3 4 5 5 10 10 10 11 11");
	a2.SetElement(13, 2);
	verify2("-1 -1 -1 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 4 5 5 10 10 10 11 11");
	a2.SetElement(18, 1);
	verify2("-1 -1 -1 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 1 5 5 10 10 10 11 11");
	a2.SetElement(18, 5);
	verify2("-1 -1 -1 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11");
	a2.SetElement(1, 0);
	verify2("0 -1 -1 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11");
	a2.SetElement(3, 0);
	verify2("0 -1 0 -1 -1 1 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11");
	a2.SetElement(10, -1);
	verify2("0 -1 0 -1 -1 1 1 1 1 -1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11");

	a2.QuickSort(QuickSortCompareLongs);
	verify2("-1 -1 -1 -1 0 0 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11");

	a2.RemoveNextElements(4,2);
	verify2("-1 -1 -1 0 1 1 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11");
	a2.RemoveNextElements(5,2);
	verify2("-1 -1 -1 0 1 1 2 2 2 2 3 3 3 5 5 5 10 10 10 11 11");
	a2.RemoveNextElements(8,2);
	verify2("-1 -1 -1 0 1 1 2 2 3 3 3 5 5 5 10 10 10 11 11");
	a2.RemoveNextElements(1,4);
	verify2("1 1 2 2 3 3 3 5 5 5 10 10 10 11 11");
	a2.InsertElementsAtIndex(3,3,2);
	verify2("1 1 3 3 2 2 3 3 3 5 5 5 10 10 10 11 11");
	a2.RemoveNextElements(5,6);
	verify2("1 1 3 3 5 5 10 10 10 11 11");

	JIndex runIndex, firstInRun;
	JBoolean ok = a2.FindRun(8, &runIndex, &firstInRun);
	assert( ok && runIndex == 4 && firstInRun == 7 );

	ok = a2.FindRun(8, 7, &runIndex, &firstInRun);
	assert( ok && runIndex == 4 && firstInRun == 7 );

	ok = a2.FindRun(7, 9, &runIndex, &firstInRun);
	assert( ok && runIndex == 4 && firstInRun == 7 );

	ok = a2.FindRun(9, 3, &runIndex, &firstInRun);
	assert( ok && runIndex == 2 && firstInRun == 3 );

	ok = a2.FindRun(3, 1, &runIndex, &firstInRun);
	assert( ok && runIndex == 1 && firstInRun == 1 );

	ok = a2.FindRun(1, 11, &runIndex, &firstInRun);
	assert( ok && runIndex == 5 && firstInRun == 10 );

	ok = a2.FindRun(11, 2, &runIndex, &firstInRun);
	assert( ok && runIndex == 1 && firstInRun == 1 );

	runIndex   = 2;
	firstInRun = 3;
	a2.InsertElementsAtIndex(5, 3, 2, &runIndex, &firstInRun);
	verify2("1 1 3 3 3 3 5 5 10 10 10 11 11");
	assert( runIndex == 2 && firstInRun == 3 );

	runIndex   = 2;
	firstInRun = 3;
	a2.InsertElementsAtIndex(5, 2, 2, &runIndex, &firstInRun);
	verify2("1 1 3 3 2 2 3 3 5 5 10 10 10 11 11");
	assert( runIndex == 3 && firstInRun == 5 );

	runIndex   = 7;
	firstInRun = 14;
	a2.InsertElementsAtIndex(17, 2, 2, &runIndex, &firstInRun);
	verify2("1 1 3 3 2 2 3 3 5 5 10 10 10 11 11 2 2");
	assert( runIndex == 8 && firstInRun == 16 );

	runIndex   = 4;
	firstInRun = 7;
	a2.RemoveNextElements(8, 4, &runIndex, &firstInRun);
	verify2("1 1 3 3 2 2 3 10 10 11 11 2 2");
	assert( runIndex == 5 && firstInRun == 8 );

	runIndex   = 3;
	firstInRun = 5;
	a2.RemoveNextElements(5, 2, &runIndex, &firstInRun);
	verify2("1 1 3 3 3 10 10 11 11 2 2");
	assert( runIndex == 2 && firstInRun == 3 );

	a2.SetNextElements(5, 2, 5);
	verify2("1 1 3 3 5 5 10 11 11 2 2");

// test InsertSlice()

	a1.RemoveAll();
	a1.AppendElements(1, 3);
	a1.AppendElements(2, 3);
	a1.AppendElements(3, 3);
	a1.AppendElements(4, 3);
	a1.AppendElements(5, 3);

	JRunArray<long> a3;
	a3.AppendElements(1, 3);
	a3.AppendElements(2, 3);
	a3.AppendElements(3, 3);
	a3.AppendElements(4, 3);
	a3.AppendElements(5, 3);

	a2 = a3;
	a2.InsertSlice(1, a1, 1, 1);
	verify2("1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(1, a1, 1, 2);
	verify2("1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(1, a1, 1, 3);
	verify2("1 1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(1, a1, 1, 4);
	verify2("1 1 1 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(1, a1, 1, 5);
	verify2("1 1 1 2 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(1, a1, 1, 6);
	verify2("1 1 1 2 2 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(1, a1, 1, 7);
	verify2("1 1 1 2 2 2 3 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(2, a1, 1, 1);
	verify2("1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(2, a1, 1, 2);
	verify2("1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(2, a1, 1, 3);
	verify2("1 1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(2, a1, 1, 4);
	verify2("1 1 1 1 2 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(2, a1, 1, 5);
	verify2("1 1 1 1 2 2 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(2, a1, 1, 6);
	verify2("1 1 1 1 2 2 2 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(2, a1, 1, 7);
	verify2("1 1 1 1 2 2 2 3 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(3, a1, 1, 1);
	verify2("1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(3, a1, 1, 2);
	verify2("1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(3, a1, 1, 3);
	verify2("1 1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(3, a1, 1, 4);
	verify2("1 1 1 1 1 2 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(3, a1, 1, 5);
	verify2("1 1 1 1 1 2 2 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(3, a1, 1, 6);
	verify2("1 1 1 1 1 2 2 2 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(3, a1, 1, 7);
	verify2("1 1 1 1 1 2 2 2 3 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(4, a1, 1, 1);
	verify2("1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(4, a1, 1, 2);
	verify2("1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(4, a1, 1, 3);
	verify2("1 1 1 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(4, a1, 1, 4);
	verify2("1 1 1 1 1 1 2 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(4, a1, 1, 5);
	verify2("1 1 1 1 1 1 2 2 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(4, a1, 1, 6);
	verify2("1 1 1 1 1 1 2 2 2 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(4, a1, 1, 7);
	verify2("1 1 1 1 1 1 2 2 2 3 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(5, a1, 1, 1);
	verify2("1 1 1 2 1 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(5, a1, 1, 2);
	verify2("1 1 1 2 1 1 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(5, a1, 1, 3);
	verify2("1 1 1 2 1 1 1 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(5, a1, 1, 4);
	verify2("1 1 1 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(5, a1, 1, 5);
	verify2("1 1 1 2 1 1 1 2 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(5, a1, 1, 6);
	verify2("1 1 1 2 1 1 1 2 2 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(5, a1, 1, 7);
	verify2("1 1 1 2 1 1 1 2 2 2 3 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(7, a1, 1, 1);
	verify2("1 1 1 2 2 2 1 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(7, a1, 1, 2);
	verify2("1 1 1 2 2 2 1 1 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(7, a1, 1, 3);
	verify2("1 1 1 2 2 2 1 1 1 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(7, a1, 1, 4);
	verify2("1 1 1 2 2 2 1 1 1 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(7, a1, 1, 5);
	verify2("1 1 1 2 2 2 1 1 1 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(7, a1, 1, 6);
	verify2("1 1 1 2 2 2 1 1 1 2 2 2 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.InsertSlice(7, a1, 1, 7);
	verify2("1 1 1 2 2 2 1 1 1 2 2 2 3 3 3 3 4 4 4 5 5 5");

	a2 = a3;
	a2.AppendSlice(a1, 15, 15);
	verify2("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 5");

	a2 = a3;
	a2.AppendSlice(a1, 14, 15);
	verify2("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 5 5");

	a2 = a3;
	a2.AppendSlice(a1, 13, 15);
	verify2("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 5 5 5");

	a2 = a3;
	a2.AppendSlice(a1, 12, 15);
	verify2("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 4 5 5 5");

	a2 = a3;
	a2.AppendSlice(a1, 8, 15);
	verify2("1 1 1 2 2 2 3 3 3 4 4 4 5 5 5 3 3 4 4 4 5 5 5");

	cout << endl;
	cout << "No error output means the remaining tests passed." << endl;

	return 0;
}

void
PrintArray
	(
	ostream&					output,
	JRunArrayIterator<long>&	iter
	)
{
	long i;
	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter.Next(&i))
		{
		output << i << ' ';
		}
	output << endl;
}

void
VerifyContents
	(
	JRunArrayIterator<long>&	iter,
	const JCharacter*			expectedStr,
	const int					line
	)
{
	std::ostringstream data;
	PrintArray(data, iter);
	const JString s = data.str();
	if (!JCompareMaxN(s, expectedStr, strlen(expectedStr), kJTrue))
		{
		cerr << endl;
		cerr << "Error on line " << line << ':' << endl;
		cerr << expectedStr << endl;
		s.Print(cerr);
		cerr << endl;
		cerr << endl;
		}
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

int
QuickSortCompareLongs
	(
	const void* a,
	const void* b
	)
{
	return (*(long*)a - *(long*)b);
}

JInteger
GetValue
	(
	const long&	data
	)
{
	return data;
}
