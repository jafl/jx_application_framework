/******************************************************************************
 JMMArrayTable.cpp

	A JMMTable implemented with JArrays.

	BASE CLASS = JMMTable

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include "JMMArrayTable.h"
#include "JMMRecord.h"
#include "jAssert.h"

	const JSize blockSize = 5000;

/******************************************************************************
 Constructor

 *****************************************************************************/

JMMArrayTable::JMMArrayTable
	(
	JMemoryManager* manager,
	const bool      recordDelete
	)
	:
	JMMTable(manager),
	itsAllocatedTable(nullptr),
	itsAllocatedBytes(0),
	itsDeletedTable(nullptr),
	itsDeletedCount(0),
	itsSnapshotID(0)
{
	itsAllocatedTable = jnew JArray<JMMRecord>(blockSize);

	if (recordDelete)
	{
		itsDeletedTable = jnew JArray<JMMRecord>(blockSize);
	}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JMMArrayTable::~JMMArrayTable()
{
	jdelete itsAllocatedTable;
	itsAllocatedTable = nullptr;

	jdelete itsDeletedTable;
	itsDeletedTable = nullptr;
}

/******************************************************************************
 GetAllocatedCount (virtual)

 *****************************************************************************/

JSize
JMMArrayTable::GetAllocatedCount() const
{
	return itsAllocatedTable->GetItemCount();
}

/******************************************************************************
 GetAllocatedBytes (virtual)

 *****************************************************************************/

JSize
JMMArrayTable::GetAllocatedBytes() const
{
	return itsAllocatedBytes;
}

/******************************************************************************
 GetDeletedCount (virtual)

	Returns zero if the table is not recording deletions.

 *****************************************************************************/

JSize
JMMArrayTable::GetDeletedCount() const
{
	if (itsDeletedTable != nullptr)
	{
		return itsDeletedTable->GetItemCount();
	}
	else
	{
		return itsDeletedCount;
	}
}

/******************************************************************************
 GetTotalCount (virtual)

 *****************************************************************************/

JSize
JMMArrayTable::GetTotalCount() const
{
	return GetAllocatedCount() + GetDeletedCount();
}

/******************************************************************************
 PrintAllocated (virtual)

 *****************************************************************************/

void
JMMArrayTable::PrintAllocated
	(
	const JMemoryManager::RecordFilter& filter
	)
	const
{
	std::cout << "\nAllocated block statistics:" << std::endl;

	std::cout << "\nAllocated user memory:" << std::endl;

	JMemoryManager::RecordFilter f(filter);
	f.includeInternal = false;

	for (const auto& thisRecord : *itsAllocatedTable)
	{
		if (f.Match(thisRecord))
		{
			PrintAllocatedRecord(thisRecord);
		}
	}

	if (filter.includeInternal)
	{
		std::cout << "\nThe following blocks are probably owned by the memory manager"
			 << "\nand *should* still be allocated--please report all cases of user"
			 << "\nallocated memory showing up on this list!" << std::endl;

		for (const auto& thisRecord : *itsAllocatedTable)
		{
			if ( thisRecord.IsManagerMemory() )
			{
				PrintAllocatedRecord(thisRecord);
			}
		}
	}
}

/******************************************************************************
 StreamAllocatedForDebug (virtual)

 *****************************************************************************/

void
JMMArrayTable::StreamAllocatedForDebug
	(
	std::ostream&						output,
	const JMemoryManager::RecordFilter&	filter
	)
	const
{
	for (const auto& thisRecord : *itsAllocatedTable)
	{
		if (filter.Match(thisRecord))
		{
			output << ' ' << JBoolToString(true);
			output << ' ';
			thisRecord.StreamForDebug(output);
		}
	}

	output << ' ' << JBoolToString(false);
}

/******************************************************************************
 StreamAllocationSizeHistogram (virtual)

 *****************************************************************************/

void
JMMArrayTable::StreamAllocationSizeHistogram
	(
	std::ostream&						output,
	const JMemoryManager::RecordFilter&	filter
	)
	const
{
	JSize histo[ JMemoryManager::kHistogramSlotCount ];
	memset(histo, 0, sizeof(histo));

	for (const auto& thisRecord : *itsAllocatedTable)
	{
		if (filter.Match(thisRecord))
		{
			AddToHistogram(thisRecord, histo);
		}
	}

	StreamHistogram(output, histo);
}

/******************************************************************************
 SaveSnapshot (virtual)

 *****************************************************************************/

void
JMMArrayTable::SaveSnapshot()
{
	itsSnapshotID = 0;

	for (const auto& thisRecord : *itsAllocatedTable)
	{
		itsSnapshotID = JMax(itsSnapshotID, thisRecord.GetID());
	}
}

/******************************************************************************
 StreamSnapshotDiffForDebug (virtual)

 *****************************************************************************/

void
JMMArrayTable::StreamSnapshotDiffForDebug
	(
	std::ostream&						output,
	const JMemoryManager::RecordFilter&	filter
	)
	const
{
	for (const auto& thisRecord : *itsAllocatedTable)
	{
		if (thisRecord.GetID() > itsSnapshotID && filter.Match(thisRecord))
		{
			output << ' ' << JBoolToString(true);
			output << ' ';
			thisRecord.StreamForDebug(output);
		}
	}

	output << ' ' << JBoolToString(false);
}

/******************************************************************************
 _AddNewRecord (virtual protected)

 *****************************************************************************/

void
JMMArrayTable::_AddNewRecord
	(
	const JMMRecord& record,
	const bool       checkDoubleAllocation
	)
{
	JIndex index = FindDeletedBlock( record.GetAddress() );	// check if address is being reused
	if (index > 0)
	{
		itsDeletedTable->RemoveItem(index);
	}

	index = 0;
	if (checkDoubleAllocation)
	{
		index = FindAllocatedBlock( record.GetAddress() );
	}

	if (index == 0)
	{
		// Append because new allocations tend to be free'd the fastest
		itsAllocatedTable->AppendItem(record);
	}
	else
	{
		JMMRecord thisRecord = itsAllocatedTable->GetItem(index);
		itsAllocatedBytes   -= thisRecord.GetSize();

		NotifyMultipleAllocation(record, thisRecord);

		// Might as well trust malloc--the table should never have duplicate
		// entries!
		itsAllocatedTable->SetItem(index, record);
	}

	itsAllocatedBytes += record.GetSize();
}

/******************************************************************************
 _SetRecordDeleted

 *****************************************************************************/

bool
JMMArrayTable::_SetRecordDeleted
	(
	JMMRecord*       record,
	const void*      block,
	const JUtf8Byte* file,
	const JUInt32    line,
	const bool       isArray
	)
{
	JSize index = FindAllocatedBlock(block);

	if (index != 0)
	{
		JMMRecord thisRecord = itsAllocatedTable->GetItem(index);
		thisRecord.SetDeleteLocation(file, line, isArray);
		itsAllocatedBytes -= thisRecord.GetSize();

		if (!thisRecord.IsArrayNew() && isArray)
		{
			NotifyObjectDeletedAsArray(thisRecord);
		}
		else if (thisRecord.IsArrayNew() && !isArray)
		{
			NotifyArrayDeletedAsObject(thisRecord);
		}

		itsAllocatedTable->RemoveItem(index);
		if (itsDeletedTable != nullptr)
		{
			itsDeletedTable->AppendItem(thisRecord);
		}
		else
		{
			itsDeletedCount++;
		}

		*record = thisRecord;
		return true;
	}
	else
	{
		// Because the array is searched backwards, if it finds a block it
		// will be the most recent deallocation at that address
		index = FindDeletedBlock(block);
		if (index == 0)
		{
			NotifyUnallocatedDeletion(file, line, isArray);
		}
		else
		{
			JMMRecord thisRecord = itsDeletedTable->GetItem(index);

			NotifyMultipleDeletion(thisRecord, file, line, isArray);
		}

		return false;
	}
}

/******************************************************************************
 FindAllocatedBlock (private)

	Returns list index, or zero on failure.

 *****************************************************************************/

JSize
JMMArrayTable::FindAllocatedBlock
	(
	const void* block
	)
	const
{
	JSize allocatedCount = itsAllocatedTable->GetItemCount();
	for (JSize i=allocatedCount;i>=1;i--)
	{
		const JMMRecord thisRecord = itsAllocatedTable->GetItem(i);
		if (thisRecord.GetAddress() == block)
		{
			return i;
		}
	}

	return 0;
}

/******************************************************************************
 FindDeletedBlock (private)

	Returns list index, or zero on failure.

 *****************************************************************************/

JSize
JMMArrayTable::FindDeletedBlock
	(
	const void* block
	)
	const
{
	if (itsDeletedTable != nullptr)
	{
		JSize deletedCount = itsDeletedTable->GetItemCount();
		for (JSize i=deletedCount;i>=1;i--)
		{
			const JMMRecord thisRecord = itsDeletedTable->GetItem(i);
			if (thisRecord.GetAddress() == block)
			{
				return i;
			}
		}
	}

	return 0;
}
