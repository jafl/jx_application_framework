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

JTEST(Exercise)
{
	JRunArray<long> a1;
	JBroadcastTester snoop1(&a1);
	JAssertTrue(a1.IsEmpty());
	JAssertEqual(0, a1.GetElementCount());

	long i;
	for (long j : { 1,2,3,4,5 })
		{
		snoop1.Expect(JListT::kElementsInserted);
		a1.AppendElement(j);
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
	verify("-1 1 1 1 1 2 3 3 3 4 5 5 10", a2);

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
