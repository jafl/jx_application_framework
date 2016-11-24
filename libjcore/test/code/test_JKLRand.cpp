/******************************************************************************
 testrand.cc

	Test random number code.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <JKLRand.h>
#include <jassert_simple.h>

	JInt32 jKLInt32List[] =
		{
		0x12345678,
		0x75432777,
		0xcd305e6a,
		0x25dbfac1,
		0x4b5c952c,
		0x84de0e9b,
		0xe2aa733e,
		0xea0f8185,
		0xf2d08520,
		0xa63075ff,
		0x81cf8b52,
		0x207db289,
		0x00 // Sentinel
		};

	long jKLLongList[] =
		{
		0,
		2360,
		2785,
		8196,
		6679,
		3841,
		6218,
		3437,
		6400,
		5078,
		5818,
		0 // Sentinel
		};

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(UniformInt32)
{
	JKLRand randGen( jKLInt32List[0] );
	JIndex i = 1;
	while (jKLInt32List[i] != 0)
		{
		const JInt32 thisVal = randGen.UniformInt32();
		JAssertEqual(jKLInt32List[i], thisVal);
		i++;
		}
}

JTEST(UniformClosedProb)
{
	JKLRand randGen( jKLLongList[0] );
	JIndex i=1;
	while (jKLLongList[i] != 0)
		{
		const long thisVal = randGen.UniformLong(0, 10000);
		JAssertEqual(jKLLongList[i], thisVal);
		i++;
		}
}

JTEST(UniformClosedProbRange)
{
	JKLRand randGen;
	for (JIndex i=1; i<=1000000; i++)
		{
		const double thisVal = randGen.UniformClosedProb();
		JAssertTrue(0.0 <= thisVal && thisVal <= 1.0);
		}
}

JTEST(UniformDoubleRange)
{
	JKLRand randGen;
	for (JIndex i=1; i<=1000000; i++)
		{
		const double thisVal = randGen.UniformDouble(-100.0, 100.0);
		JAssertTrue(-100.0 <= thisVal && thisVal <= 100.0);
		}
}
