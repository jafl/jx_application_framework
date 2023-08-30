/******************************************************************************
 testnew.cpp

	Test program for the JCore memory manager system.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

// To make JCore shut up
#include "JUserNotification.h"
#include "JCreateProgressDisplay.h"

	JUserNotification* gUserNotification = nullptr;
	JCreateProgressDisplay* gCreatePG = nullptr;
	const JCharacter* kJMonospaceFontName = nullptr;
	const JCharacter* kJDefaultFontName = nullptr;


#include "JMemoryManager.h"
#include "JMMErrorPrinter.h"

#include "JArray.h"
#include <iomanip>
#include "jAssert.h"

/******************************************************************************
 main

 *****************************************************************************/

int
main()
{
	std::cout << "   Testing object jnew/jdelete, should never fail" << std::endl;
	long* aLong = jnew long;
	jdelete aLong;

	std::cout << "\n   Testing array jnew/jdelete, should never fail" << std::endl;
	aLong = jnew long[15];
	jdelete[] aLong;

	std::cout << "\n   Testing initialization value" << std::endl;
	const JSize blockSize = 4;
	aLong = jnew long[blockSize];
	std::cout << std::setbase(16) << "\n   Block of longs initialized to: ";
	for (JUnsignedOffset i=0;i<blockSize;i++)
	{
		std::cout << aLong[i];
	}
	std::cout << std::setbase(10) << std::endl;
	jdelete[] aLong;
	std::cout << "\n   Testing shredding value (slightly dangerous!)" << std::endl;
	std::cout << std::setbase(16) << "\n   Block of longs shredded to: ";
	for (JUnsignedOffset i=0;i<blockSize;i++)
	{
		std::cout << aLong[i];
	}
	std::cout << std::setbase(10) << std::endl;

//	JMemoryManager::Instance()->DiscardDeletionRecords();

	std::cout << "\n   Deleting unallocated memory, will fail if allocations are being"
		 << "\n   recorded, will succeed with system-dependent results (dumps core"
		 << "\n   on Linux) if allocations are not being recorded" << std::endl;
	double* aDouble = (double*) 0x34343;
	jdelete aDouble;
	jdelete[] aDouble;

//	JMemoryManager::Instance()->CancelRecordAllocated();

	std::cout << "\n   Multiple deleting object.  Should fail as 'delete unallocated'"
		 << "\n   but *not* dump core if allocations are being recorded, or should fail"
		 << "\n   as double deletion if deallocations are being recorded.  Otherwise,"
		 << "\n   will succeed with system-dependent results" << std::endl;
	float* aFloat = jnew float;



	jdelete aFloat;



	jdelete aFloat;

	jdelete[] aFloat;

	std::cout << "\n   Multiple deleting array.  Should fail as 'delete unallocated'"
		 << "\n   but *not* dump core if allocations are being recorded, or should fail"
		 << "\n   as double deletion if deallocations are being recorded.  Otherwise,"
		 << "\n   will succeed with system-dependent results" << std::endl;
	aFloat = jnew float[5];



	jdelete[] aFloat;



	jdelete[] aFloat;

	jdelete aFloat;

	std::cout << "\n   Array deleting object allocation, should fail if"
		 << "\n   allocations are being recorded" << std::endl;
	int* aInt = jnew int;
	jdelete[] aInt;

	std::cout << "\n   Object deleting array allocation, should fail if"
		 << "\n   allocations are being recorded" << std::endl;
	aDouble = jnew double[10];
	jdelete aDouble;

	std::cout << "\nDeleting nullptr as object, should not fail unless it has been disallowed" << std::endl;
	aFloat = nullptr;
	jdelete aFloat;
	std::cout << "\nNow deleting as array, ***WHY DOES THIS FAIL?!?***" << std::endl;
	jdelete[] aFloat;

	JMemoryManager::Instance()->PrintMemoryStats();

	std::cout << "\nAllocating two more memory blocks, should count properly" << std::endl;
	char* aChar = jnew char[30];
	aChar = jnew char[30];

	JMemoryManager::Instance()->PrintMemoryStats();

	std::cout << "\nAllocating JArray--how many blocks does it need?" << std::endl;

	JArray<int>* array = jnew JArray<int>;

	JMemoryManager::Instance()->PrintMemoryStats();

	std::cout << "\nTwo user blocks plus a JArray are undeleted--should show up on final stats" << std::endl;

	return 0;
}
