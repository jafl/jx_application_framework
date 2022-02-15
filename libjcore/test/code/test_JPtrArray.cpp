/******************************************************************************
 test_JPtrArray.cpp

	Program to test JPtrArray and JPtrArrayIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JBroadcastTester.h>
#include <jx-af/jcore/jAssert.h>

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
	JAssertEqual(0, a1.GetElementCount());

	long i;
	for (i=1;i<=5;i++)
	{
		stringPtr = jnew JString;
		assert( stringPtr != nullptr );
		stringPtr->Append(JUtf8Character('0' + i));
		snoop1.Expect(JListT::kElementsInserted);
		a1.Append(stringPtr);
	}
	stringPtr = nullptr;

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(5, a1.GetElementCount());

	snoop1.Expect(JListT::kElementsSwapped);
	a1.SwapElements(2,5);

	stringPtr = jnew JString("1", 0);
	assert( stringPtr != nullptr );

	snoop1.Expect(JListT::kElementsInserted);
	a1.InsertElementAtIndex(3, stringPtr);

	snoop1.Expect(JListT::kElementMoved);
	a1.MoveElementToIndex(3, a1.GetElementCount());

	JPtrArrayIterator<JString> iter(&a1,kJIteratorStartAtBeginning);

	snoop1.Expect(JListT::kElementsRemoved);
{
	const JUtf8Byte* expect[] = { "1", "5", "3", "2", "1" };
	long j                    = 0;
	while (iter.Next(&stringPtr))
	{
		JAssertStringsEqual(expect[j], *stringPtr);
		j++;

		if (*stringPtr == "5")
		{
			stringPtr = a1.GetElement(4);
			a1.Remove(stringPtr);
			jdelete stringPtr;
			stringPtr = nullptr;
		}
	}
	JAssertEqual(5, j);
}

	JIndex index;
	stringPtr = a1.GetElement(4);
	JAssertTrue(a1.Find(stringPtr, &index));
	JAssertEqual(4, index);

	snoop1.Expect(JListT::kElementsRemoved);
{
	const JUtf8Byte* expect[] = { "1", "2", "3", "5", "1" };
	long j                    = 0;
	while (iter.Prev(&stringPtr))
	{
		JAssertStringsEqual(expect[j], *stringPtr);
		j++;

		if (*stringPtr == "5")
		{
			a1.DeleteElement(4);
			stringPtr = nullptr;
		}
	}
	JAssertEqual(5, j);
}

	JAssertFalse(a1.IsEmpty());
	JAssertEqual(4, a1.GetElementCount());

	JPtrArray<JString> a2(a1, JPtrArrayT::kForgetAll);
{
	JBroadcastTester snoop2(&a2);

	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetElementCount());

	JPtrArrayIterator<JString> iter2(&a2, kJIteratorStartAtEnd);
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

	snoop2.Expect(JListT::kElementsRemoved);
	a2.RemoveAll();

	JAssertTrue(a2.IsEmpty());
	JAssertEqual(0, a2.GetElementCount());

	snoop2.Expect(JListT::kElementsInserted);
	snoop2.Expect(JListT::kElementsInserted);
	snoop2.Expect(JListT::kElementsInserted);
	snoop2.Expect(JListT::kElementsInserted);

	a2.CopyPointers(a1, JPtrArrayT::kForgetAll, false);

	JAssertFalse(a2.IsEmpty());
	JAssertEqual(4, a2.GetElementCount());
{
	const JUtf8Byte* expect[] = { "1", "3", "5", "1" };
	long j                    = 0;
	iter2.MoveTo(kJIteratorStartAtEnd, 0);
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
	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
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
	JAssertEqual(0, a2.GetElementCount());

	snoop1.Expect(JListT::kElementsRemoved);
	a1.DeleteAll();

	JAssertTrue(a1.IsEmpty());
	JAssertEqual(0, a1.GetElementCount());
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

	JAssertStringsEqual("foo", *a2.GetElement(1));
	JAssertStringsEqual("bar", *a2.GetElement(2));
	JAssertStringsEqual("baz", *a2.GetElement(3));
}
