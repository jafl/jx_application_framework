/******************************************************************************
 JMMTable.cpp

	An abstract base class for a JMemoryManager memory table.

	BASE CLASS = <NONE>

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include <JMMTable.h>
#include <JMemoryManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JMMTable::JMMTable
	(
	JMemoryManager* manager
	)
	:
	itsManager(manager)
{
	assert(itsManager != NULL);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JMMTable::~JMMTable()
{
}

/******************************************************************************
 AddNewRecord

 *****************************************************************************/

void
JMMTable::AddNewRecord
	(
	const JMMRecord& record,
	const JBoolean checkDoubleAllocation
	)
{
	BeginRecursiveBlock();
	_AddNewRecord(record, checkDoubleAllocation);
	EndRecursiveBlock();
}

/******************************************************************************
 SetRecordDeleted

	If the block is currently allocated, *record is set to its record and kJTrue
	is return.  If not, *record is unchanged and kJFalse is returned.

 *****************************************************************************/

JBoolean
JMMTable::SetRecordDeleted
	(
	JMMRecord*       record,
	const void*      block,
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	BeginRecursiveBlock();
	JBoolean wasAllocated = _SetRecordDeleted(record, block, file, line, isArray);
	EndRecursiveBlock();
	return wasAllocated;
}

/******************************************************************************
 CancelRecordDeallocated

 *****************************************************************************/

void
JMMTable::CancelRecordDeallocated()
{
	BeginRecursiveBlock();
	_CancelRecordDeallocated();
	EndRecursiveBlock();
}

/******************************************************************************
 GetAllocatedCount (pure virtual)

 *****************************************************************************/

/******************************************************************************
 GetDeletedCount (pure virtual)

 *****************************************************************************/

/******************************************************************************
 GetTotalCount (pure virtual)

 *****************************************************************************/

/******************************************************************************
 PrintAllocated (pure virtual)

 *****************************************************************************/

/******************************************************************************
 _CancelRecordDeallocated (pure virtual)

 *****************************************************************************/

/******************************************************************************
 _AddNewRecord (pure virtual protected)

 *****************************************************************************/

/******************************************************************************
 _SetRecordDeleted (pure virtual protected)

 *****************************************************************************/

/******************************************************************************
 BeginRecursiveBlock (protected)

 *****************************************************************************/

void
JMMTable::BeginRecursiveBlock()
{
	itsManager->BeginRecursiveBlock();
}

/******************************************************************************
 EndRecursiveBlock (protected)

 *****************************************************************************/

void
JMMTable::EndRecursiveBlock()
{
	itsManager->EndRecursiveBlock();
}

/******************************************************************************
 NotifyObjectDeletedAsArray (protected)

 *****************************************************************************/

void
JMMTable::NotifyObjectDeletedAsArray
	(
	const JMMRecord& record
	)
{
	itsManager->HandleObjectDeletedAsArray(record);
}

/******************************************************************************
 NotifyArrayDeletedAsObject (protected)

 *****************************************************************************/

void
JMMTable::NotifyArrayDeletedAsObject
	(
	const JMMRecord& record
	)
{
	itsManager->HandleArrayDeletedAsObject(record);
}

/******************************************************************************
 NotifyUnallocatedDeletion (protected)

 *****************************************************************************/

void
JMMTable::NotifyUnallocatedDeletion
	(
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	itsManager->HandleUnallocatedDeletion(file, line, isArray);
}

/******************************************************************************
 NotifyMultipleDeletion (protected)

 *****************************************************************************/

void
JMMTable::NotifyMultipleDeletion
	(
	const JMMRecord& firstRecord,
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	itsManager->HandleMultipleDeletion(firstRecord, file, line, isArray);
}

/******************************************************************************
 NotifyMultipleAllocation (protected)

 *****************************************************************************/

void
JMMTable::NotifyMultipleAllocation
	(
	const JMMRecord& thisRecord,
	const JMMRecord& firstRecord
	)
{
	itsManager->HandleMultipleAllocation(thisRecord, firstRecord);
}

/******************************************************************************
 PrintAllocatedRecord (protected)

 *****************************************************************************/

void
JMMTable::PrintAllocatedRecord
	(
	const JMMRecord& record
	)
	const
{
	cout << "\n                ID: " << record.GetID();
	cout << "\n           Address: " << record.GetAddress();
	cout << "\n              Data: " << (unsigned char*) record.GetAddress();
	cout << "\n              Size: " << record.GetSize();
	cout << "\n        New method: " << record.NewTypeName();
	cout << "\n         New'ed at: " << record.GetNewFile() << ":" << record.GetNewLine();
	cout << "\n      Verification: ";
	if (record.GetDeleteFile() != NULL || record.GetDeleteLine() != 0)
		{
		cout << "*** manager error: allocated status inconsistent, possibly not really allocated";
		}
	else
		{
		cout << "allocated status consistent, probably correct";
		}
	cout << endl;
}

/******************************************************************************
 AddToHistogram (protected)

 *****************************************************************************/

void
JMMTable::AddToHistogram
	(
	const JMMRecord&	record,
	JSize				histo[ JMemoryManager::kHistogramSlotCount ]
	)
	const
{
	const JSize size = record.GetSize();

	JUInt64 max = 0x000000001ULL;
	max       <<= 32 - (JMemoryManager::kHistogramSlotCount - 1);

	JIndex slot    = 0;
	JBoolean found = kJFalse;
	while (slot < JMemoryManager::kHistogramSlotCount - 1)
		{
		if (size <= max)
			{
			histo[ slot ]++;
			found = kJTrue;
			break;
			}
		max <<= 1;
		max  &= 0x1FFFFFFFFULL;
		slot++;
		}

	if (!found)
		{
		histo[ JMemoryManager::kHistogramSlotCount-1 ]++;
		}
}

/******************************************************************************
 StreamHistogram (protected)

	The dual function is in jx_memory_debugger.

 *****************************************************************************/

void
JMMTable::StreamHistogram
	(
	ostream&	output,
	const JSize	histo[ JMemoryManager::kHistogramSlotCount ]
	)
	const
{
	output << JMemoryManager::kHistogramSlotCount;

	for (JIndex i=0; i<JMemoryManager::kHistogramSlotCount; i++)
		{
		output << ' ' << histo[i];
		}
}
