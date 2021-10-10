#ifndef _H_JMMTable
#define _H_JMMTable

/******************************************************************************
 JMMTable.h

	Interface for the JMMTable class.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "jx-af/jcore/JMemoryManager.h"

class JMMRecord;

class JMMTable
{
public:

	JMMTable(JMemoryManager* manager);
	virtual ~JMMTable();

	void AddNewRecord(const JMMRecord& record,
					  const bool checkDoubleAllocation);

	bool SetRecordDeleted(JMMRecord* record, const void* block,
						  const JUtf8Byte* file, const JUInt32 line,
						  const bool isArray);

	virtual JSize GetAllocatedCount() const = 0;
	virtual JSize GetAllocatedBytes() const = 0;
	virtual JSize GetDeletedCount() const = 0;
	virtual JSize GetTotalCount() const = 0;

	virtual void PrintAllocated(const bool printInternal = false) const = 0;
	virtual void StreamAllocatedForDebug(std::ostream& output, const JMemoryManager::RecordFilter& filter) const = 0;
	virtual void StreamAllocationSizeHistogram(std::ostream& output) const = 0;

protected:

	virtual void _AddNewRecord(const JMMRecord& record,
							   const bool checkDoubleAllocation) = 0;

	virtual bool _SetRecordDeleted(JMMRecord* record, const void* block,
								   const JUtf8Byte* file, const JUInt32 line,
								   const bool isArray) = 0;

	void BeginRecursiveBlock();
	void EndRecursiveBlock();

	// Message notification

	void NotifyObjectDeletedAsArray(const JMMRecord& record);
	void NotifyArrayDeletedAsObject(const JMMRecord& record);

	void NotifyUnallocatedDeletion(const JUtf8Byte* file, const JUInt32 line,
								   const bool isArray);
	void NotifyMultipleDeletion(const JMMRecord& firstRecord, const JUtf8Byte* file,
								const JUInt32 line, const bool isArray);

	void NotifyMultipleAllocation(const JMMRecord& thisRecord,
								  const JMMRecord& firstRecord);

	void PrintAllocatedRecord(const JMMRecord& record) const;
	void AddToHistogram(const JMMRecord& record, JSize histo[JMemoryManager::kHistogramSlotCount]) const;
	void StreamHistogram(std::ostream& output, const JSize histo[JMemoryManager::kHistogramSlotCount]) const;

private:

	JMemoryManager* const itsManager;

private:

	// not allowed

	JMMTable(const JMMTable&) = delete;
	JMMTable& operator=(const JMMTable&) = delete;
};

#endif
