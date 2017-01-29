/******************************************************************************
 test_JRunArray.cc

	Program to test JRunArray and JRunArrayIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JRunArray.h>
#include <JBroadcastTester.h>
#include <sstream>
#include <jAssert.h>

#define verify2(str)	VerifyContents(iter2, str, __LINE__)

int main()
{
	return JUnitTestManager::Execute();
}

void
PrintArray
	(
	std::ostream&				output,
	JRunArrayIterator<long>&	iter
	)
{
	long i;
	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter.Next(&i))
		{
		output << i << ' ';
		}
	output << std::endl;
}

void
VerifyContents
	(
	JRunArrayIterator<long>&	iter,
	const JUtf8Byte*			expectedStr,
	const int					line
	)
{
	std::ostringstream data;
	PrintArray(data, iter);
	JString s;
	s = data.str();
	s.TrimWhitespace();
	JAreEqual(expectedStr, s, __FILE__, line);
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

JTEST(Exercise)
{
	JRunArray<long> a1;
	JBroadcastTester snoop1(&a1);
	JAssertTrue(a1.IsEmpty());
	JAssertEqual(0, a1.GetElementCount());

	long i,j;

	for (i=1;i<=5;i++)
		{
		snoop1.Expect(JListT::kElementsInserted);
		a1.AppendElement(i);
		}
	// 1 2 3 4 5

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(5, a1.GetElementCount());

	snoop1.Expect(JListT::kElementChanged);
	a1.SetElement(5,1);
	// 1 2 3 4 1

	snoop1.Expect(JListT::kElementsSwapped);
	a1.SwapElements(2,5);
	// 1 1 3 4 2

	snoop1.Expect(JListT::kElementsInserted);
	a1.InsertElementAtIndex(3,1);
	// 1 1 1 3 4 2

	snoop1.Expect(JListT::kElementChanged);
	a1.SetElement(2,2);
	// 1 2 1 3 4 2

	snoop1.Expect(JListT::kElementChanged);
	a1.SetElement(2,1);
	// 1 1 1 3 4 2

	snoop1.Expect(JListT::kElementMoved);
	a1.MoveElementToIndex(3,a1.GetElementCount());
	// 1 1 3 4 2 1

	JRunArrayIterator<long> iter(&a1,kJIteratorStartAtBeginning);
	{
	long expect[] = { 1, 3, 3, 2, 1 };
	long j        = 0;
	while (iter.Next(&i))
		{
		JAssertEqual(expect[j], i);
		j++;

		if (j == 1)
			{
			snoop1.Expect(JListT::kElementChanged);
			a1.SetElement(2,3);
			// 1 3 3 4 2 1
			}
		else if (j == 3)
			{
			snoop1.Expect(JListT::kElementsRemoved);
			a1.RemoveElement(4);
			// 1 3 3 2 1
			}
		}
	JAssertEqual(5, j);
	}

	{
	long expect[] = { 1, 2, 3, 1, 1 };
	long j        = 0;
	while (iter.Prev(&i))
		{
		JAssertEqual(expect[j], i);
		j++;

		if (j == 3)
			{
			snoop1.Expect(JListT::kElementChanged);
			a1.SetElement(2,1);
			// 1 1 3 2 1
			snoop1.Expect(JListT::kElementsRemoved);
			a1.RemoveElement(4);
			// 1 1 3 1
			}
		}
	JAssertEqual(5, j);
	}

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(4, a1.GetElementCount());

	JRunArray<long> a2 = a1;
{
	JBroadcastTester snoop2(&a2);
	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetElementCount());

	JRunArrayIterator<long> iter2(&a2,kJIteratorStartAtEnd);
	{
	long expect[] = { 1, 3, 1, 1 };
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
	{
	long expect[] = { 1, 3, 1, 1 };
	long j        = 0;
	iter2.MoveTo(kJIteratorStartAtEnd, 0);
	while (iter2.Prev(&i))
		{
		JAssertEqual(expect[j], i);
		j++;
		}
	JAssertEqual(4, j);
	}
}
	JRunArrayIterator<long> iter2(&a2);

// test sort ascending

	a2.AppendElement(3);
	a2.AppendElement(5);
	a2.AppendElement(2);
	a2.AppendElement(1);
	a2.AppendElement(5);

	a2.SetCompareFunction(CompareLongs);
	a2.SetSortOrder(JListT::kSortAscending);
	a2.Sort();

	verify2("1 1 1 1 2 3 3 5 5");

// test insertion sort
	{
	JBoolean expect[] = { kJTrue, kJFalse, kJFalse, kJFalse };
	long element[] = {3, -1, 10, 4};
	const long eCount = sizeof(element)/sizeof(long);

	JBoolean isDuplicate;
	long k = 0;
	for (i=0; i<eCount; i++)
		{
		const JIndex j = a2.GetInsertionSortIndex(element[i], &isDuplicate);
		a2.InsertElementAtIndex(j, element[i]);
		JAssertEqual(expect[k], isDuplicate);
		k++;
		}
	JAssertEqual(4, k);
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
		JAssertTrue(
			a2.SearchSorted(element[i], JListT::kFirstMatch, &p1) == found[i] &&
			pos[i] == p1 &&
			a2.SearchSorted(element[i], JListT::kLastMatch, &p2) == found[i] &&
			pos[i] == p2 &&
			a2.SearchSorted(element[i], JListT::kAnyMatch, &p3) == found[i] &&
			pos[i] == p3);
		}

	JIndex p4=0, p5=0;
	JAssertTrue(a2.SearchSorted(1, JListT::kFirstMatch, &p4));
	JAssertEqual(2, p4);
	JAssertTrue(a2.SearchSorted(1, JListT::kLastMatch, &p5));
	JAssertEqual(5, p5);
	}

// test sort descending

	a2.SetSortOrder(JListT::kSortDescending);
	a2.Sort();

	verify2("10 5 5 4 3 3 3 2 1 1 1 1 -1");

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
		JAssertTrue(
			a2.SearchSorted(element[i], JListT::kFirstMatch, &p6) == found[i] &&
			pos[i] == p6 &&
			a2.SearchSorted(element[i], JListT::kLastMatch, &p7) == found[i] &&
			pos[i] == p7 &&
			a2.SearchSorted(element[i], JListT::kAnyMatch, &p8) == found[i] &&
			pos[i] == p8);
		}

	JIndex p4=0, p5=0;
	JAssertTrue(a2.SearchSorted(3, JListT::kFirstMatch, &p4));
	JAssertEqual(5, p4);
	JAssertTrue(a2.SearchSorted(3, JListT::kLastMatch, &p5));
	JAssertEqual(7, p5);
	}

	a2.QuickSort(QuickSortCompareLongs);
	verify2("-1 1 1 1 1 2 3 3 3 4 5 5 10");

// test SumElements()

	JInteger sum = a2.SumElements(1,1, GetValue);
	JAssertEqual(-1, sum);
	sum = a2.SumElements(1,2, GetValue);
	JAssertEqual(0, sum);
	sum = a2.SumElements(1,5, GetValue);
	JAssertEqual(3, sum);
	sum = a2.SumElements(1,8, GetValue);
	JAssertEqual(11, sum);
	sum = a2.SumElements(2,8, GetValue);
	JAssertEqual(12, sum);
	sum = a2.SumElements(4,8, GetValue);
	JAssertEqual(10, sum);
	sum = a2.SumElements(4,9, GetValue);
	JAssertEqual(13, sum);
	sum = a2.SumElements(6,6, GetValue);
	JAssertEqual(2, sum);

	JIndex endIndex;
	JInteger trueSum;
	JAssertTrue(a2.FindPositiveSum(1, 1, &endIndex, &trueSum, GetValue));
	JAssertEqual(4, endIndex);
	JAssertEqual(1, trueSum);
	JAssertTrue(a2.FindPositiveSum(3, 1, &endIndex, &trueSum, GetValue));
	JAssertEqual(6, endIndex);
	JAssertEqual(3, trueSum);
	JAssertTrue(a2.FindPositiveSum(11, 1, &endIndex, &trueSum, GetValue));
	JAssertEqual(9, endIndex);
	JAssertEqual(11, trueSum);
	JAssertTrue(a2.FindPositiveSum(10, 1, &endIndex, &trueSum, GetValue));
	JAssertEqual(8, endIndex);
	JAssertEqual(8, trueSum);
	JAssertTrue(a2.FindPositiveSum(9, 2, &endIndex, &trueSum, GetValue));
	JAssertEqual(8, endIndex);
	JAssertEqual(9, trueSum);
	JAssertTrue(a2.FindPositiveSum(10, 2, &endIndex, &trueSum, GetValue));
	JAssertEqual(8, endIndex);
	JAssertEqual(9, trueSum);
	JAssertTrue(a2.FindPositiveSum(15, 2, &endIndex, &trueSum, GetValue));
	JAssertEqual(10, endIndex);
	JAssertEqual(15, trueSum);
	JAssertTrue(a2.FindPositiveSum(4, 4, &endIndex, &trueSum, GetValue));
	JAssertEqual(7, endIndex);
	JAssertEqual(4, trueSum);
	JAssertTrue(a2.FindPositiveSum(6, 4, &endIndex, &trueSum, GetValue));
	JAssertEqual(7, endIndex);
	JAssertEqual(4, trueSum);
	JAssertTrue(a2.FindPositiveSum(7, 4, &endIndex, &trueSum, GetValue));
	JAssertEqual(8, endIndex);
	JAssertEqual(7, trueSum);
	JAssertFalse(a2.FindPositiveSum(20, 12, &endIndex, &trueSum, GetValue));
	JAssertEqual(13, endIndex);
	JAssertEqual(5, trueSum);
	JAssertTrue(a2.FindPositiveSum(8, 13, &endIndex, &trueSum, GetValue));
	JAssertEqual(13, endIndex);
	JAssertEqual(0, trueSum);

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
	JAssertTrue(a2.FindRun(8, &runIndex, &firstInRun));
	JAssertEqual(4, runIndex);
	JAssertEqual(7, firstInRun);

	JAssertTrue(a2.FindRun(8, 7, &runIndex, &firstInRun));
	JAssertEqual(4, runIndex);
	JAssertEqual(7, firstInRun);

	JAssertTrue(a2.FindRun(7, 9, &runIndex, &firstInRun));
	JAssertEqual(4, runIndex);
	JAssertEqual(7, firstInRun);

	JAssertTrue(a2.FindRun(9, 3, &runIndex, &firstInRun));
	JAssertEqual(2, runIndex);
	JAssertEqual(3, firstInRun);

	JAssertTrue(a2.FindRun(3, 1, &runIndex, &firstInRun));
	JAssertEqual(1, runIndex);
	JAssertEqual(1, firstInRun);

	JAssertTrue(a2.FindRun(1, 11, &runIndex, &firstInRun));
	JAssertEqual(5, runIndex);
	JAssertEqual(10, firstInRun);

	JAssertTrue(a2.FindRun(11, 2, &runIndex, &firstInRun));
	JAssertEqual(1, runIndex);
	JAssertEqual(1, firstInRun);

	runIndex   = 2;
	firstInRun = 3;
	a2.InsertElementsAtIndex(5, 3, 2, &runIndex, &firstInRun);
	verify2("1 1 3 3 3 3 5 5 10 10 10 11 11");
	JAssertEqual(2, runIndex);
	JAssertEqual(3, firstInRun);

	runIndex   = 2;
	firstInRun = 3;
	a2.InsertElementsAtIndex(5, 2, 2, &runIndex, &firstInRun);
	verify2("1 1 3 3 2 2 3 3 5 5 10 10 10 11 11");
	JAssertEqual(3, runIndex);
	JAssertEqual(5, firstInRun);

	runIndex   = 7;
	firstInRun = 14;
	a2.InsertElementsAtIndex(17, 2, 2, &runIndex, &firstInRun);
	verify2("1 1 3 3 2 2 3 3 5 5 10 10 10 11 11 2 2");
	JAssertEqual(8, runIndex);
	JAssertEqual(16, firstInRun);

	runIndex   = 4;
	firstInRun = 7;
	a2.RemoveNextElements(8, 4, &runIndex, &firstInRun);
	verify2("1 1 3 3 2 2 3 10 10 11 11 2 2");
	JAssertEqual(5, runIndex);
	JAssertEqual(8, firstInRun);

	runIndex   = 3;
	firstInRun = 5;
	a2.RemoveNextElements(5, 2, &runIndex, &firstInRun);
	verify2("1 1 3 3 3 10 10 11 11 2 2");
	JAssertEqual(2, runIndex);
	JAssertEqual(3, firstInRun);

	a2.SetNextElements(5, 2, 5);
	verify2("1 1 3 3 5 5 10 11 11 2 2");

// test InsertSlice()

	snoop1.Expect(JListT::kElementsRemoved);
	snoop1.Expect(JListT::kElementsInserted);
	snoop1.Expect(JListT::kElementsInserted);
	snoop1.Expect(JListT::kElementsInserted);
	snoop1.Expect(JListT::kElementsInserted);
	snoop1.Expect(JListT::kElementsInserted);

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
}
