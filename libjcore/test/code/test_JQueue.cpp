/******************************************************************************
 test_JQueue.cc

	Program to test JQueue class.

	Written by John Lindal.

 ******************************************************************************/

#include <JQueue.h>
#include <JArray.h>
#include <jCommandLine.h>
#include <jAssert.h>

int main()
{
	long i;

JQueue<long, JArray<long> > a1;									// constructor

	cout << "queue a1 created" << endl << endl;

	cout << "a1 itemCount should be 0" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	for (i=1;i<=5;i++)
		{
		a1.Append(i);
		}

	cout << "a1 itemCount should be 5" << endl;
	cout << "a1 itemCount = " << a1.GetElementCount() << endl << endl;

	JWaitForReturn();

JQueue<long, JArray<long> > a2 = a1;							// copy constructor

	cout << "queue a2 created from a1" << endl << endl;

	cout << "a2 itemCount should be 5" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	cout << "display should be:  1 2 3 4 5" << endl;

	while (!a1.IsEmpty())
		{
		i = a1.GetNext();
		cout << i << ' ';
		}
	cout << endl;

	cout << "a1 itemCount should be 0" << endl;
	cout << "a1 itemCount=" << a1.GetElementCount() << endl << endl;

	cout << "display should be:  1 2" << endl;

	do
		{
		i = a2.GetNext();
		cout << i << ' ';
		}
		while (i < 2);

	cout << endl;

	JWaitForReturn();

	cout << "display should be:  3" << endl;

	cout << a2.PeekNext() << endl;

	cout << "a2 itemCount should be 3" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	a2.Discard(2);

	cout << "a2 itemCount should be 1" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	a2.Flush();

	cout << "a2 itemCount should be 0" << endl;
	cout << "a2 itemCount=" << a2.GetElementCount() << endl << endl;

	return 0;
}
