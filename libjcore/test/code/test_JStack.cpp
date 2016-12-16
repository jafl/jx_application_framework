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

	std::cout << "stack a1 created" << std::endl << std::endl;

	std::cout << "a1 itemCount should be 0" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	for (i=1;i<=5;i++)
		{
		a1.Push(i);
		}

	std::cout << "a1 itemCount should be 5" << std::endl;
	std::cout << "a1 itemCount = " << a1.GetElementCount() << std::endl << std::endl;

	JWaitForReturn();

JStack<long, JArray<long> > a2 = a1;							// copy constructor

	std::cout << "stack a2 created from a1" << std::endl << std::endl;

	std::cout << "a2 itemCount should be 5" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	std::cout << "display should be:  5 4 3 2 1" << std::endl;

	while (!a1.IsEmpty())
		{
		i = a1.Pop();
		std::cout << i << ' ';
		}
	std::cout << std::endl;

	std::cout << "a1 itemCount should be 0" << std::endl;
	std::cout << "a1 itemCount=" << a1.GetElementCount() << std::endl << std::endl;

	std::cout << "display should be:  5 4" << std::endl;

	do
		{
		i = a2.Pop();
		std::cout << i << ' ';
		}
		while (i > 4);

	std::cout << std::endl;

	std::cout << "a2 itemCount should be 3" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	a2.Unwind(2);

	std::cout << "a2 itemCount should be 1" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	a2.Clear();

	std::cout << "a2 itemCount should be 0" << std::endl;
	std::cout << "a2 itemCount=" << a2.GetElementCount() << std::endl << std::endl;

	return 0;
}
