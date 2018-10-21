/******************************************************************************
 testmemcpy.cpp

	Test memcpy speed vs for loop vs unrolled loop.

	Written by John Lindal.

 *****************************************************************************/

#include <iostream>
#include <string.h>
#include <time.h>

const long cycleCount = 10000000000;

/******************************************************************************
 main

 *****************************************************************************/

int
main()
{
	std::cout << "Starting test..." << std::endl;

	char s1[] = { 0, 0, 0, 0 };
	char s2[] = { 0, 0, 0, 0 };

	std::cout << "memcpy:" << std::endl;

	for (long j=1; j<=4; j++)
		{
		const time_t t1 = time(nullptr);

		for (long i=0; i<cycleCount; i++)
			{
			memcpy(s2, s1, j);
			}

		const time_t t2 = time(nullptr);
		std::cout << j << " byte(s): " << (t2 - t1) / (double) cycleCount << std::endl;
		}

	std::cout << "for loop:" << std::endl;

	for (long j=1; j<=4; j++)
		{
		const time_t t1 = time(nullptr);

		for (long i=0; i<cycleCount; i++)
			{
			for (long k=0; k<j; k++)
				{
				s2[k] = s1[k];
				}
			}

		const time_t t2 = time(nullptr);
		std::cout << j << " byte(s): " << (t2 - t1) / (double) cycleCount << std::endl;
		}

	return 0;
}
/*

Starting test...
memcpy:
1 byte(s): 3.4e-09
2 byte(s): 3e-09
3 byte(s): 4.2e-09
4 byte(s): 3.8e-09
for loop:
1 byte(s): 1.8e-09
2 byte(s): 2.8e-09
3 byte(s): 4.3e-09
4 byte(s): 5.5e-09

No discernable difference between memcpy and for loop.

 */
