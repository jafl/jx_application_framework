/******************************************************************************
 testrandperiod.cpp

	Tests the period of the JKLRandInt Knuth-Morris random number generator.
	Because this takes so long it won't be run very often, it is a program of
	its own rather than part of testrand.

	Copyright (C) 1997 by Dustin Laurence.

 *****************************************************************************/

#include <jRand.h>
#include <time.h>
#include <jAssert.h>

	const unsigned long callibrationCycles = 10000000UL;

/******************************************************************************
 main

 *****************************************************************************/

int
main()
{
	std::cout << "\nBeginning Knuth-Morris period test.  Be prepared to wait a while." << std::endl;

	JInt32 seed = 0;
	unsigned long period = 0;

	std::cout << "\n   Running " << callibrationCycles
			  << " iterations to estimate running time." << std::endl;

	const time_t startCallibrationTime = time(nullptr);
	const clock_t startCallibrationClock = clock();

	do
	{
		seed = JKLRandInt32(seed);
		++period;
	}
		while(period < callibrationCycles);

	const time_t  endCallibrationTime = time(nullptr);
	const clock_t endCallibrationClock = clock();

	const double deltaCallibrationClock = (1.0*endCallibrationClock-startCallibrationClock)
										 / CLOCKS_PER_SEC;
	time_t deltaCallibrationTime = endCallibrationTime-startCallibrationTime;
	// time() rounds off strangely, try to ensure an over- rather than under-estimate
	while (deltaCallibrationTime < deltaCallibrationClock)
	{
		++deltaCallibrationTime;
	}
	std::cout << "\n   Callibration run time : " << deltaCallibrationClock
			  << " processor sec in " << deltaCallibrationTime << " real sec." << std::endl;

	std::cout << "      (if the real run time was small, the following estimate\n"
				 "       can be grossly inaccurate)" << std::endl;

	const double maxPeriod = 1.0*ULONG_MAX + 1.0;
	const double timeMultiple = maxPeriod/callibrationCycles;

	std::cout << "   Running time of full test should be no more than:\n      "
			  << timeMultiple*deltaCallibrationClock << " processor sec = "
			  << timeMultiple*deltaCallibrationClock/60.0 << " processor min in\n      "
			  << timeMultiple*deltaCallibrationTime << " real sec = "
			  << timeMultiple*deltaCallibrationTime/60.0 << " real min" << std::endl;

	period = 0;
	JInt32 startSeed = seed;

	std::cout << "\n   Running full period test." << std::endl;

	const time_t startTime = time(nullptr);
	const clock_t startClock = clock();

	do
	{
		seed = JKLRandInt32(seed);
		++period;
	}
		while(seed != startSeed);

	clock_t endClock = clock();
	time_t endTime = time(nullptr);

	std::cout << "   Full period test finished." << std::endl;

	const time_t deltaTime = endTime-startTime;
	const double deltaClock = (1.0*endClock-startClock) / CLOCKS_PER_SEC;

	std::cout << "\n   Actual run time : " << deltaClock
			  << " processor sec in " << deltaTime << " real sec." << std::endl;

	if (period != 0)
	{
		std::cout << "\n   JKLRandInt32 has a period of " << period << std::endl;
		std::cout << "   Maximum possible period is 2^32 ~ " << maxPeriod
				  << ",\n   so the period is " << period/maxPeriod << " of ideal." << std::endl;
	}
	else
	{
		std::cout << "\n   JKLRandInt32 has a period of 2^32 ~ " << maxPeriod
				  << ", the maximum possible." << std::endl;
	}

	std::cout << "\nFinished Knuth-Morris test.  Whew, that worked up a sweat.\n" << std::endl;

	return 0;
}
