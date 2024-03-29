/******************************************************************************
 test_JAliasArray.cpp

	Program to test JAliasArray class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "jListTestUtil.h"
#include "JAliasArray.h"
#include "JArray.h"
#include "jAssert.h"

#define verifyalias(str, list)	JAssertStringsEqual(str, JPrintAliasArray(list));

int main()
{
	return JTestManager::Execute();
}

template <class T>
JString
JPrintAliasArray
	(
	const JAliasArray<T>& a
	)
{
	std::ostringstream data;

	const JSize count = a.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		data << a.GetItem(i) << ' ';
	}

	JString s(data.str());
	s.TrimWhitespace();
	return s;
}

std::weak_ordering
CompareLongs
	(
	const long& a,
	const long& b
	)
{
	return JIntToWeakOrdering(a-b);
}

int
qsortCompare(const void* e1, const void* e2)
{
	return (*(long*)e1 - *(long*)e2);
}

JTEST(Construct)
{
	JArray<long> a;
	a.SetCompareFunction(CompareLongs);
	a.SetSortOrder(JListT::kSortAscending);

	JAliasArray<long> aa(&a, CompareLongs, JListT::kSortDescending);

	JAssertTrue(aa.IsEmpty());
	JAssertEqual(0, aa.GetItemCount());
	JAssertFalse(aa.IndexValid(1));

	JSetList("1 2 3 4 5", &a);

	JAssertFalse(aa.IsEmpty());
	JAssertEqual(5, aa.GetItemCount());
	JAssertTrue(aa.IndexValid(1));
	JAssertTrue(aa.IndexValid(5));
	JAssertFalse(aa.IndexValid(6));
}

JTEST(Prepend)
{
	JArray<long> a;
	JAliasArray<long> aa(&a, CompareLongs, JListT::kSortDescending);

	for (long j : { 5,4,3,2,1 })
	{
		a.PrependItem(j);
	}

	verify("1 2 3 4 5", a);
	verifyalias("5 4 3 2 1", aa);

	JAssertEqual(4, aa.GetItem(2));
	JAssertEqual(2, aa.GetItemFromEnd(2));
	JAssertEqual(5, aa.GetFirstItem());
	JAssertEqual(1, aa.GetLastItem());
}

JTEST(Append)
{
	JArray<long> a;
	JAliasArray<long> aa(&a, CompareLongs, JListT::kSortDescending);

	for (long j : { 1,2,3,4,5 })
	{
		a.AppendItem(j);
	}

	verify("1 2 3 4 5", a);
	verifyalias("5 4 3 2 1", aa);

	JAssertEqual(4, aa.GetItem(2));
	JAssertEqual(2, aa.GetItemFromEnd(2));
	JAssertEqual(5, aa.GetFirstItem());
	JAssertEqual(1, aa.GetLastItem());
}

JTEST(Insert)
{
	JArray<long> a;
	JAliasArray<long> aa(&a, CompareLongs, JListT::kSortAscending);

	JSetList("5 4 3 2 1", &a);

	a.InsertItemAtIndex(1, 6);
	verify("6 5 4 3 2 1", a);
	verifyalias("1 2 3 4 5 6", aa);

	a.InsertItemAtIndex(4, -1);
	verify("6 5 4 -1 3 2 1", a);
	verifyalias("-1 1 2 3 4 5 6", aa);

	a.InsertItemAtIndex(7, 0);
	verify("6 5 4 -1 3 2 0 1", a);
	verifyalias("-1 0 1 2 3 4 5 6", aa);

	a.InsertItemAtIndex(9, 10);
	verify("6 5 4 -1 3 2 0 1 10", a);
	verifyalias("-1 0 1 2 3 4 5 6 10", aa);
}

JTEST(Remove)
{
	JArray<long> a;
	JAliasArray<long> aa(&a, CompareLongs, JListT::kSortDescending);

	JSetList("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15", &a);

	a.RemoveItem(7);
	verify("1 2 3 4 5 6 8 9 10 11 12 13 14 15", a);
	verifyalias("15 14 13 12 11 10 9 8 6 5 4 3 2 1", aa);

	a.RemoveItem(1);
	verify("2 3 4 5 6 8 9 10 11 12 13 14 15", a);
	verifyalias("15 14 13 12 11 10 9 8 6 5 4 3 2", aa);

	a.RemoveItem(13);
	verify("2 3 4 5 6 8 9 10 11 12 13 14", a);
	verifyalias("14 13 12 11 10 9 8 6 5 4 3 2", aa);

	a.RemoveNextItems(8, 3);
	verify("2 3 4 5 6 8 9 13 14", a);
	verifyalias("14 13 9 8 6 5 4 3 2", aa);

	a.RemovePrevItems(5, 2);
	verify("2 3 4 8 9 13 14", a);
	verifyalias("14 13 9 8 4 3 2", aa);

	a.RemoveAll();
	JAssertTrue(a.IsEmpty());
	JAssertTrue(aa.IsEmpty());
}

JTEST(Move)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JSetList("1 2 3 4 5", &a);

	JAliasArray<long> aa(&a, CompareLongs, JListT::kSortDescending);
	verifyalias("5 4 3 2 1", aa);

	a.MoveItemToIndex(3, a.GetIndexFromEnd(1));
	verify("1 2 4 5 3", a);
	verifyalias("5 4 3 2 1", aa);

	a.MoveItemToIndex(3, 1);
	verify("4 1 2 5 3", a);
	verifyalias("5 4 3 2 1", aa);

	a.MoveItemToIndex(4, 2);
	verify("4 5 1 2 3", a);
	verifyalias("5 4 3 2 1", aa);
}

JTEST(Swap)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JSetList("1 2 3 4 5", &a);

	JAliasArray<long> aa(&a, CompareLongs, JListT::kSortDescending);
	verifyalias("5 4 3 2 1", aa);

	a.SwapItems(2,5);
	verify("1 5 3 4 2", a);
	verifyalias("5 4 3 2 1", aa);
}

JTEST(Sort)
{
	JArray<long> a(1);		// ridiculous block size to really exercise resizer
	JSetList("3 1 5 1 1", &a);

	JAliasArray<long> aa(&a, CompareLongs, JListT::kSortDescending);
	verifyalias("5 3 1 1 1", aa);

// test sort ascending

	a.SetCompareFunction(CompareLongs);
	a.SetSortOrder(JListT::kSortAscending);
	a.Sort();
	verify("1 1 1 3 5", a);
	verifyalias("5 3 1 1 1", aa);
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
		a.InsertItemAtIndex(j, element[i]);
		JAssertEqual(expect[i], (bool) isDuplicate);
	}
}
	verify("-1 1 1 1 3 3 5 10", a);
	verifyalias("10 5 3 3 1 1 1 -1", aa);

	JAssertTrue(a.InsertSorted(4));
	JAssertFalse(a.InsertSorted(3, false));
	verify("-1 1 1 1 3 3 4 5 10", a);
	verifyalias("10 5 4 3 3 1 1 1 -1", aa);

// test sort descending

	a.SetSortOrder(JListT::kSortDescending);
	a.Sort();
	verify("10 5 4 3 3 1 1 1 -1", a);
	verifyalias("10 5 4 3 3 1 1 1 -1", aa);

// quick sort

	a.QuickSort(qsortCompare);
	verify("-1 1 1 1 3 3 4 5 10", a);
	verifyalias("10 5 4 3 3 1 1 1 -1", aa);
}
