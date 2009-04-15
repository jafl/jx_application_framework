/******************************************************************************
 test_JStack.cc

	Program to test JStack class.

	Written by John Lindal.

 ******************************************************************************/

#include <JStack.h>
#include <JArray.h>
#include <jCommandLine.h>
#include <jAssert.h>

int main()
{
	long i;

JStack<long, JArray<long> > a1;									// constructor

	cout << "stack a1 created" << endl << endl;

	cout << "a1 itemCount should be 0" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	for (i=1;i<=5;i++)
		{
		a1.Push(i);
		}

	cout << "a1 itemCount should be 5" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	JWaitForReturn();

JStack<long, JArray<long> > a2 = a1;							// copy constructor

	cout << "stack a2 created from a1" << endl << endl;

	cout << "a2 itemCount should be 5" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	cout << "display should be:  5 4 3 2 1" << endl;

	while (!a1.IsEmpty())
		{
		i = a1.Pop();
		cout << i << ' ';
		}
	cout << endl;

	cout << "a1 itemCount should be 0" << endl;
	cout << "a1 itemCount=" << a1.GetElementCount() << endl << endl;

	cout << "display should be:  5 4" << endl;

	do
		{
		i = a2.Pop();
		cout << i << ' ';
		}
		while (i > 4);

	cout << endl;

	cout << "a2 itemCount should be 3" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	a2.Unwind(2);

	cout << "a2 itemCount should be 1" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	a2.Clear();

	cout << "a2 itemCount should be 0" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	return 0;
}
