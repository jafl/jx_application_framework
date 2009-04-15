/******************************************************************************
 test_JPtrArray.cc

	Program to test JPtrArray and JPtrArrayIterator classes.

	Written by John Lindal.

 ******************************************************************************/

#include <JPtrArray-JString.h>
#include <JBroadcastSnooper.h>
#include <jCommandLine.h>
#include <jAssert.h>

int main()
{
	JString* stringPtr = NULL;

JPtrArray<JString> a1(JPtrArrayT::kDeleteAll);					// constructor

	cout << "array a1 created" << endl << endl;

JBroadcastSnooper snoop1(&a1);

	cout << "a1 address: " << (void*) &a1 << endl;

	cout << "a1 itemCount should be 0" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	long i;
	for (i=1;i<=5;i++)
		{
		stringPtr = new JString("0");
		assert( stringPtr != NULL );
		stringPtr->SetCharacter(1, '0' + i);
		a1.Append(stringPtr);
		}
	stringPtr = NULL;

	cout << "a1 itemCount should be 5" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	JWaitForReturn();

	a1.SwapElements(2,5);

	stringPtr = new JString("1");
	assert( stringPtr != NULL );
	a1.InsertAtIndex(3, stringPtr);

	a1.MoveElementToIndex(3, a1.GetElementCount());

JPtrArrayIterator<JString> iter(&a1,kJIteratorStartAtBeginning);

	cout << "display should be:  1 5 3 2 1" << endl;

	while (iter.Next(&stringPtr))
		{
		cout << (*stringPtr) << ' ';

		if (*stringPtr == "5")
			{
			stringPtr = a1.NthElement(4);
			a1.Remove(stringPtr);
			delete stringPtr;
			stringPtr = NULL;
			}
		}
	cout << endl << endl;

	JIndex index;
	stringPtr = a1.NthElement(4);
	cout << "Searching for 2:  found = " << a1.Find(stringPtr, &index) << endl;
	cout << "2 should be at index 4" << endl;
	cout << "2 is at index " << index << endl << endl;

	cout << "display should be:  1 2 3 5 1" << endl;

	while (iter.Prev(&stringPtr))
		{
		cout << (*stringPtr) << ' ';

		if (*stringPtr == "5")
			{
			a1.DeleteElement(4);
			stringPtr = NULL;
			}
		}
	cout << endl;

	cout << "a1 itemCount should be 4" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	JWaitForReturn();

JPtrArray<JString> a2(a1, JPtrArrayT::kForgetAll, kJFalse);		// copy constructor

	cout << "array a2 created from a1" << endl << endl;

JBroadcastSnooper snoop2(&a2);

	cout << "a2 address: " << (void*) &a2 << endl;

	cout << "a2 itemCount should be 4" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

JPtrArrayIterator<JString> iter2(&a2, kJIteratorStartAtEnd);

	cout << "display should be: 1 3 5 1" << endl;

	while (iter2.Prev(&stringPtr))
		{
		cout << (*stringPtr) << ' ';
		}
	cout << endl;

	a2.RemoveAll();

	cout << "a2 itemCount should be 0" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	a2.CopyPointers(a1, JPtrArrayT::kForgetAll, kJFalse);		// assignment operator

	cout << "array a2 assigned from a1" << endl << endl;

	cout << "a2 itemCount should be 4" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	cout << "display should be: 1 3 5 1" << endl;

	iter2.MoveTo(kJIteratorStartAtEnd, 0);
	while (iter2.Prev(&stringPtr))
		{
		cout << (*stringPtr) << ' ';
		}
	cout << endl;

	JWaitForReturn();

// test sort ascending

	a2.SetCompareFunction(JCompareStringsCaseInsensitive);
	a2.SetSortOrder(JOrderedSetT::kSortAscending);
	a2.Sort();

	cout << "display should be: 1 1 3 5" << endl;

	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter2.Next(&stringPtr))
		{
		cout << (*stringPtr) << ' ';
		}
	cout << endl << endl;

// test RemoveAll(), DeleteAll()

	a2.RemoveAll();

	cout << "a2 itemCount should be 0" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	a1.DeleteAll();

	cout << "a1 itemCount should be 0" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	return 0;
}
