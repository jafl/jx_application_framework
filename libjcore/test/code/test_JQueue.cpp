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

	std::cout << "queue a1 created" << std::endl << std::endl;

	std::cout << "a1 itemCount should be 0" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	for (i=1;i<=5;i++)
		{
		a1.Append(i);
		}

	std::cout << "a1 itemCount should be 5" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	JWaitForReturn();

JQueue<long, JArray<long> > a2 = a1;							// copy constructor

	std::cout << "queue a2 created from a1" << std::endl << std::endl;

	std::cout << "a2 itemCount should be 5" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	std::cout << "display should be:  1 2 3 4 5" << std::endl;

	while (!a1.IsEmpty())
		{
		i = a1.GetNext();
		std::cout << i << ' ';
		}
	std::cout << std::endl;

	std::cout << "a1 itemCount should be 0" << std::endl;
	std::cout << "a1 itemCount=" << a1.GetElementCount() << std::endl << std::endl;

	std::cout << "display should be:  1 2" << std::endl;

	do
		{
		i = a2.GetNext();
		std::cout << i << ' ';
		}
		while (i < 2);

	std::cout << std::endl;

	JWaitForReturn();

	std::cout << "display should be:  3" << std::endl;

	std::cout << a2.PeekNext() << std::endl;

	std::cout << "a2 itemCount should be 3" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	a2.Discard(2);

	std::cout << "a2 itemCount should be 1" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	a2.Flush();

	std::cout << "a2 itemCount should be 0" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	return 0;
}
