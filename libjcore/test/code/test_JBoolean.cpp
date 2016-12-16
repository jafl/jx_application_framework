#include <jTypes.h>
#include <jCommandLine.h>
#include <stdio.h>
#include <jAssert.h>

int main()
{
	std::cout << "True:  " << kJTrue  << std::endl;
	std::cout << "False: " << kJFalse << std::endl;

	std::cout << "should see:" << std::endl << "TT" << std::endl;

	JBoolean b;
/*	b = 3;
	if (b)				std::cout << 'T';
	if (b == kJTrue)		std::cout << 'T';
	if (!b)				std::cout << 'F';
	if (b == kJFalse)	std::cout << 'F';

	b = 0;
	if (!b)				std::cout << 'T';
	if (b == kJFalse)	std::cout << 'T';
	if (b)				std::cout << 'F';
	if (b == kJTrue)		std::cout << 'F';

	b = (5 > 3);
	if (b)				std::cout << 'T';
	if (b == kJTrue)		std::cout << 'T';
	if (!b)				std::cout << 'F';
	if (b == kJFalse)	std::cout << 'F';
*/
	b = JF2B(0.1);
	if (b)				std::cout << 'T';
	if (b == kJTrue)		std::cout << 'T';
	if (!b)				std::cout << 'F';
	if (b == kJFalse)	std::cout << 'F';

	std::cout << std::endl;

	b = kJTrue;
	assert( b );
	b = kJFalse;
	assert( !b );

	std::cout << "Enter a boolean value: ";
	std::cin >> b;
	if (std::cin.good())
		{
		std::cout << "You entered " << b << std::endl;
		}
	else
		{
		std::cout << "You entered an invalid value." << std::endl;
		}
	JInputFinished();

	return 0;
}

//	operator void*() const
//		{
//		return (void*)(itsValue ? this : NULL);
//		}
