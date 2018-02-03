/******************************************************************************
 JMMHashTable.cpp

	A JMMTable implemented with JHashTables.

	BASE CLASS = JMMTable

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JMMRecord.h>

//Class Header
#include <JMMHashTable.h>
#include <jAssert.h>

	const JSize kInitialSize = 5000;

/******************************************************************************
 Constructor

 *****************************************************************************/

JMMHashTable::JMMHashTable
	(
	JMemoryManager* manager,
	const JBoolean  recordDelete
	)
	:
	JMMTable(manager),
	itsAllocatedTable(NULL),
	itsAllocatedBytes(0),
	itsDeletedTable(NULL),
	itsDeletedCount(0)
{
	itsAllocatedTable = jnew JHashTable<JMMRecord>(kInitialSize);
	assert(itsAllocatedTable != NULL);

	if (recordDelete)
		{
		itsDeletedTable = jnew JHashTable<JMMRecord>(kInitialSize);
		assert(itsDeletedTable != NULL);
		}
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JMMHashTable::~JMMHashTable()
{
	jdelete itsAllocatedTable;
	itsAllocatedTable = NULL;

	jdelete itsDeletedTable;
	itsDeletedTable = NULL;
}

/******************************************************************************
 GetAllocatedCount (virtual)

 *****************************************************************************/

JSize
JMMHashTable::GetAllocatedCount() const
{
	return itsAllocatedTable->GetElementCount();
}

/******************************************************************************
 GetAllocatedBytes (virtual)

 *****************************************************************************/

JSize
JMMHashTable::GetAllocatedBytes() const
{
	return itsAllocatedBytes;
}

/******************************************************************************
 GetDeletedCount (virtual)

	Returns zero if the table is not recording deletions.

 *****************************************************************************/

JSize
JMMHashTable::GetDeletedCount() const
{
	if (itsDeletedTable != NULL)
		{
		return itsDeletedTable->GetElementCount();
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
JMMHashTable::GetTotalCount() const
{
	return GetAllocatedCount() + GetDeletedCount();
}

/******************************************************************************
 PrintAllocated (virtual)

 *****************************************************************************/

void
JMMHashTable::PrintAllocated
	(
	const JBoolean printInternal // = kJFalse
	)
	const
{
	std::cout << "\nAllocated block statistics:" << std::endl;

	std::cout << "\nAllocated user memory:" << std::endl;

	JConstHashCursor<JMMRecord> cursor(itsAllocatedTable);
	JSize totalSize = 0;
	while ( cursor.NextFull() )
		{
		const JMMRecord thisRecord = cursor.GetValue();
		if ( !thisRecord.IsManagerMemory() )
			{
			PrintAllocatedRecord(thisRecord);
			totalSize += thisRecord.GetSize();
			}
		}

	std::cout << "\nTotal allocated memory:  " << totalSize << " bytes" << std::endl;

	if (printInternal)
		{
		std::cout << "\nThe following blocks are probably owned by the memory manager"
				  << "\nand *should* still be allocated--please report all cases of user"
				  << "\nallocated memory showing up on this list!" << std::endl;

		cursor.Reset();
		while ( cursor.NextFull() )
			{
			const JMMRecord thisRecord = cursor.GetValue();
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
JMMHashTable::StreamAllocatedForDebug
	(
	std::ostream&							output,
	const JMemoryManager::RecordFilter&	filter
	)
	const
{
	JConstHashCursor<JMMRecord> cursor(itsAllocatedTable);
	JSize totalSize = 0;
	while ( cursor.NextFull() )
		{
		const JMMRecord thisRecord = cursor.GetValue();
		if (filter.Match(thisRecord))
			{
			output << ' ' << kJTrue;
			output << ' ';
			thisRecord.StreamForDebug(output);
			}
		}

	output << ' ' << kJFalse;
}

/******************************************************************************
 StreamAllocationSizeHistogram (virtual)

 *****************************************************************************/

void
JMMHashTable::StreamAllocationSizeHistogram
	(
	std::ostream& output
	)
	const
{
	JSize histo[ JMemoryManager::kHistogramSlotCount ];
	bzero(histo, sizeof(histo));

	JConstHashCursor<JMMRecord> cursor(itsAllocatedTable);
	while ( cursor.NextFull() )
		{
		AddToHistogram(cursor.GetValue(), histo);
		}

	StreamHistogram(output, histo);
}

/******************************************************************************
 _CancelRecordDeallocated (virtual)

 *****************************************************************************/

void
JMMHashTable::_CancelRecordDeallocated()
{
	if (itsDeletedTable != NULL)
		{
		itsDeletedCount = itsDeletedTable->GetElementCount();

		jdelete itsDeletedTable;
		itsDeletedTable = NULL;
		}
}

/******************************************************************************
 _AddNewRecord (virtual protected)

 *****************************************************************************/

void
JMMHashTable::_AddNewRecord
	(
	const JMMRecord& record,
	const JBoolean   checkDoubleAllocation
	)
{
	JHashCursor<JMMRecord> cursor(itsAllocatedTable, reinterpret_cast<JHashValue>( record.GetAddress() ) );
	if (checkDoubleAllocation)
		{
		cursor.ForceNextMapInsertHash();
		if ( cursor.IsFull() )
			{
			JMMRecord thisRecord = cursor.GetValue();
			itsAllocatedBytes   -= thisRecord.GetSize();
			NotifyMultipleAllocation(record, thisRecord);
			}
		// Might as well trust malloc--the table should never have duplicate
		// entries!
		}
	else
		{
		cursor.ForceNextOpen();
		}
	cursor.Set(reinterpret_cast<JHashValue>( record.GetAddress() ), record);
	itsAllocatedBytes += record.GetSize();
}

/******************************************************************************
 _SetRecordDeleted

 *****************************************************************************/

JBoolean
JMMHashTable::_SetRecordDeleted
	(
	JMMRecord*       record,
	const void*      block,
	const JUtf8Byte* file,
	const JUInt32    line,
	const JBoolean   isArray
	)
{
	JHashCursor<JMMRecord> allocCursor(itsAllocatedTable, reinterpret_cast<JHashValue>(block) );
	if ( allocCursor.NextHash() )
		{
		JMMRecord thisRecord = allocCursor.GetValue();
		thisRecord.SetDeleteLocation(file, line, isArray);
		itsAllocatedBytes -= thisRecord.GetSize();

		if (!thisRecord.ArrayNew() && isArray)
			{
			NotifyObjectDeletedAsArray(thisRecord);
			}
		else if (thisRecord.ArrayNew() && !isArray)
			{
			NotifyArrayDeletedAsObject(thisRecord);
			}

		allocCursor.Remove();
		if (itsDeletedTable != NULL)
			{
			JHashCursor<JMMRecord> deallocCursor(itsDeletedTable, reinterpret_cast<JHashValue>(block) );
			deallocCursor.ForceNextOpen();
			deallocCursor.Set(reinterpret_cast<JHashValue>(block), thisRecord);
			}
		else
			{
			itsDeletedCount++;
			}

		*record = thisRecord;
		return kJTrue;
		}
	else
		{
		if (itsDeletedTable == NULL)
			{
			NotifyUnallocatedDeletion(file, line, isArray);
			}
		else
			{
			JHashCursor<JMMRecord> deallocCursor(itsDeletedTable, reinterpret_cast<JHashValue>(block) );
			if ( deallocCursor.NextHash() )
				{
				// Seek most recent deallocation at that address
				JMMRecord previousRecord = deallocCursor.GetValue();
				while ( deallocCursor.NextHash() )
					{
					JMMRecord thisRecord = deallocCursor.GetValue();
					if ( thisRecord.GetID() > previousRecord.GetID() )
						{
						previousRecord = thisRecord;
						}
					}

				NotifyMultipleDeletion(previousRecord, file, line, isArray);
				}
			else
				{
				NotifyUnallocatedDeletion(file, line, isArray);
				}
			}
		return kJFalse;
		}
}
