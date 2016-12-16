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

	std::cout << "array a1 created" << std::endl << std::endl;

JBroadcastSnooper snoop1(&a1);

	std::cout << "a1 address: " << (void*) &a1 << std::endl;

	std::cout << "a1 itemCount should be 0" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	long i;
	for (i=1;i<=5;i++)
		{
		stringPtr = jnew JString("0");
		assert( stringPtr != NULL );
		stringPtr->SetCharacter(1, '0' + i);
		a1.Append(stringPtr);
		}
	stringPtr = NULL;

	std::cout << "a1 itemCount should be 5" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	JWaitForReturn();

	a1.SwapElements(2,5);

	stringPtr = jnew JString("1");
	assert( stringPtr != NULL );
	a1.InsertAtIndex(3, stringPtr);

	a1.MoveElementToIndex(3, a1.GetElementCount());

JPtrArrayIterator<JString> iter(&a1,kJIteratorStartAtBeginning);

	std::cout << "display should be:  1 5 3 2 1" << std::endl;

	while (iter.Next(&stringPtr))
		{
		std::cout << (*stringPtr) << ' ';

		if (*stringPtr == "5")
			{
			stringPtr = a1.NthElement(4);
			a1.Remove(stringPtr);
			jdelete stringPtr;
			stringPtr = NULL;
			}
		}
	std::cout << std::endl << std::endl;

	JIndex index;
	stringPtr = a1.NthElement(4);
	std::cout << "Searching for 2:  found = " << a1.Find(stringPtr, &index) << std::endl;
	std::cout << "2 should be at index 4" << std::endl;
	std::cout << "2 is at index " << index << std::endl << std::endl;

	std::cout << "display should be:  1 2 3 5 1" << std::endl;

	while (iter.Prev(&stringPtr))
		{
		std::cout << (*stringPtr) << ' ';

		if (*stringPtr == "5")
			{
			a1.DeleteElement(4);
			stringPtr = NULL;
			}
		}
	std::cout << std::endl;

	std::cout << "a1 itemCount should be 4" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	JWaitForReturn();

JPtrArray<JString> a2(a1, JPtrArrayT::kForgetAll);		// copy constructor

	std::cout << "array a2 created from a1" << std::endl << std::endl;

JBroadcastSnooper snoop2(&a2);

	std::cout << "a2 address: " << (void*) &a2 << std::endl;

	std::cout << "a2 itemCount should be 4" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

JPtrArrayIterator<JString> iter2(&a2, kJIteratorStartAtEnd);

	std::cout << "display should be: 1 3 5 1" << std::endl;

	while (iter2.Prev(&stringPtr))
		{
		std::cout << (*stringPtr) << ' ';
		}
	std::cout << std::endl;

	a2.RemoveAll();

	std::cout << "a2 itemCount should be 0" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	a2.CopyPointers(a1, JPtrArrayT::kForgetAll, kJFalse);		// assignment operator

	std::cout << "array a2 assigned from a1" << std::endl << std::endl;

	std::cout << "a2 itemCount should be 4" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	std::cout << "display should be: 1 3 5 1" << std::endl;

	iter2.MoveTo(kJIteratorStartAtEnd, 0);
	while (iter2.Prev(&stringPtr))
		{
		std::cout << (*stringPtr) << ' ';
		}
	std::cout << std::endl;

	JWaitForReturn();

// test sort ascending

	a2.SetCompareFunction(JCompareStringsCaseInsensitive);
	a2.SetSortOrder(JOrderedSetT::kSortAscending);
	a2.Sort();

	std::cout << "display should be: 1 1 3 5" << std::endl;

	iter2.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter2.Next(&stringPtr))
		{
		std::cout << (*stringPtr) << ' ';
		}
	std::cout << std::endl << std::endl;

// test RemoveAll(), DeleteAll()

	a2.RemoveAll();

	std::cout << "a2 itemCount should be 0" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	a1.DeleteAll();

	std::cout << "a1 itemCount should be 0" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	return 0;
}
