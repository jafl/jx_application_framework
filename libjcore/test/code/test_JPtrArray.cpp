/******************************************************************************
 test_JPtrArray.cpp

	Program to test JPtrArray and JPtrArrayIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JPtrArray-JString.h"
#include "JBroadcastTester.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	JString* stringPtr = nullptr;

	JPtrArray<JString> a1(JPtrArrayT::kDeleteAll, 1); // Ridiculous block size to really exercise resizer
	JBroadcastTester snoop1(&a1);
	JAssertTrue(a1.IsEmpty());
	JAssertEqual(0, a1.GetItemCount());

	long i;
	for (i=1;i<=5;i++)
	{
		stringPtr = jnew JString;
		stringPtr->Append(JUtf8Character('0' + i));
		snoop1.Expect(JListT::kItemsInserted);
		a1.Append(stringPtr);
	}
	stringPtr = nullptr;

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(5, a1.GetItemCount());

	snoop1.Expect(JListT::kItemsSwapped);
	a1.SwapItems(2,5);

	stringPtr = jnew JString("1", 0);

	snoop1.Expect(JListT::kItemsInserted);
	a1.InsertItemAtIndex(3, stringPtr);

	snoop1.Expect(JListT::kItemMoved);
	a1.MoveItemToIndex(3, a1.GetItemCount());

	JPtrArrayIterator<JString> iter(&a1, JListT::kStartAtBeginning);

	snoop1.Expect(JListT::kItemsRemoved);
{
	const JUtf8Byte* expect[] = { "1", "5", "3", "2", "1" };
	long j                    = 0;
	while (iter.Next(&stringPtr))
	{
		JAssertStringsEqual(expect[j], *stringPtr);
		j++;

		if (*stringPtr == "5")
		{
			stringPtr = a1.GetItem(4);
			a1.Remove(stringPtr);
			jdelete stringPtr;
			stringPtr = nullptr;
		}
	}
	JAssertEqual(5, j);
}

	JIndex index;
	stringPtr = a1.GetItem(4);
	JAssertTrue(a1.Find(stringPtr, &index));
	JAssertEqual(4, index);

	snoop1.Expect(JListT::kItemsRemoved);
{
	const JUtf8Byte* expect[] = { "1", "2", "3", "5", "1" };
	long j                    = 0;
	while (iter.Prev(&stringPtr))
	{
		JAssertStringsEqual(expect[j], *stringPtr);
		j++;

		if (*stringPtr == "5")
		{
			a1.DeleteItem(4);
			stringPtr = nullptr;
		}
	}
	JAssertEqual(5, j);
}

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(4, a1.GetItemCount());

	JPtrArray<JString> a2(a1, JPtrArrayT::kForgetAll);
{
	JBroadcastTester snoop2(&a2);

	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetItemCount());

	JPtrArrayIterator<JString> iter2(&a2, JListT::kStartAtEnd);
{
	const JUtf8Byte* expect[] = { "1", "3", "5", "1" };
	long j                    = 0;
	while (iter2.Prev(&stringPtr))
	{
		JAssertStringsEqual(expect[j], *stringPtr);
		j++;
	}
	JAssertEqual(4, j);
}

	snoop2.Expect(JListT::kItemsRemoved);
	a2.RemoveAll();

	JAssertTrue(a2.IsEmpty());
	JAssertEqual(0, a2.GetItemCount());

	snoop2.Expect(JListT::kItemsInserted);
	snoop2.Expect(JListT::kItemsInserted);
	snoop2.Expect(JListT::kItemsInserted);
	snoop2.Expect(JListT::kItemsInserted);

	a2.CopyPointers(a1, JPtrArrayT::kForgetAll, false);

	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetItemCount());
{
	const JUtf8Byte* expect[] = { "1", "3", "5", "1" };
	long j                    = 0;
	iter2.MoveTo(JListT::kStartAtEnd, 0);
	while (iter2.Prev(&stringPtr))
	{
		JAssertStringsEqual(expect[j], *stringPtr);
		j++;
	}
	JAssertEqual(4, j);
}
}
	JPtrArrayIterator<JString> iter2(&a2);

	// test sort ascending

	a2.SetCompareFunction(JCompareStringsCaseInsensitive);
	a2.SetSortOrder(JListT::kSortAscending);
	a2.Sort();
{
	const JUtf8Byte* expect[] = { "1", "1", "3", "5" };
	long j                    = 0;
	iter2.MoveTo(JListT::kStartAtBeginning, 0);
	while (iter2.Next(&stringPtr))
	{
		JAssertStringsEqual(expect[j], *stringPtr);
		j++;
	}
	JAssertEqual(4, j);
}

	JString target("3", JString::kNoCopy);
	JAssertTrue(a2.SearchSorted(&target, JListT::kAnyMatch, &index));
	JAssertEqual(3, index);

	target = "1";
	JAssertTrue(a2.SearchSorted(&target, JListT::kFirstMatch, &index));
	JAssertEqual(1, index);
	JAssertTrue(a2.SearchSorted(&target, JListT::kLastMatch, &index));
	JAssertEqual(2, index);

	target = "4";
	JAssertFalse(a2.SearchSorted(&target, JListT::kAnyMatch, &index));

	bool found;
	index = a2.SearchSortedOTI(&target, JListT::kAnyMatch, &found);
	JAssertFalse(found);
	JAssertEqual(4, index);

	target = "3";
	index = a2.SearchSortedOTI(&target, JListT::kAnyMatch, &found);
	JAssertTrue(found);
	JAssertEqual(3, index);

	a2.RemoveAll();

	JAssertTrue(a2.IsEmpty());
	JAssertEqual(0, a2.GetItemCount());

	snoop1.Expect(JListT::kItemsRemoved);
	a1.DeleteAll();

	JAssertTrue(a1.IsEmpty());
	JAssertEqual(0, a1.GetItemCount());
}

JTEST(RangeBasedForLoop)
{
	JPtrArray<JString> a(JPtrArrayT::kDeleteAll);
	a.Append(JString("foo", JString::kNoCopy));
	a.Append(JString("bar", JString::kNoCopy));
	a.Append(JString("baz", JString::kNoCopy));

	JString* b[3];
	int j = 0;
	for (JString* s : a)
	{
		b[j++] = s;
	}

	JAssertStringsEqual("foo", *b[0]);
	JAssertStringsEqual("bar", *b[1]);
	JAssertStringsEqual("baz", *b[2]);
}

JTEST(MoveCtor)
{
	auto* a1 = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( a1 != nullptr );
	a1->Append(JString("foo", JString::kNoCopy));
	a1->Append(JString("bar", JString::kNoCopy));
	a1->Append(JString("baz", JString::kNoCopy));

	JPtrArray<JString> a2(std::move(*a1));

	jdelete a1;
	a1 = nullptr;

	JAssertStringsEqual("foo", *a2.GetItem(1));
	JAssertStringsEqual("bar", *a2.GetItem(2));
	JAssertStringsEqual("baz", *a2.GetItem(3));
}
