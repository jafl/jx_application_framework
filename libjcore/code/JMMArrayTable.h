#ifndef _H_JMMArrayTable
#define _H_JMMArrayTable

/******************************************************************************
 JMMArrayTable.h

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JMMTable.h>
#include <JArray.h>

class JMMArrayTable : public JMMTable
{
public:

	JMMArrayTable(JMemoryManager* manager, const JBoolean recordDelete);
	virtual ~JMMArrayTable();

	virtual JSize GetAllocatedCount() const;
	virtual JSize GetAllocatedBytes() const;
	virtual JSize GetDeletedCount() const;
	virtual JSize GetTotalCount() const;

	virtual void PrintAllocated(const JBoolean printInternal = kJFalse) const;
	virtual void StreamAllocatedForDebug(std::ostream& output, const JMemoryManager::RecordFilter& filter) const;
	virtual void StreamAllocationSizeHistogram(std::ostream& output) const;

protected:

	virtual void _CancelRecordDeallocated();

	virtual void _AddNewRecord(const JMMRecord& record,
									   const JBoolean checkDoubleAllocation);

	virtual JBoolean _SetRecordDeleted(JMMRecord* record, const void* block,
									   const JUtf8Byte* file, const JUInt32 line,
									   const JBoolean isArray);

private:

	JArray<JMMRecord>*	itsAllocatedTable;
	JSize				itsAllocatedBytes;
	JArray<JMMRecord>*	itsDeletedTable;
	JSize				itsDeletedCount;

private:

	JSize FindAllocatedBlock(const void* block) const;
	JSize FindDeletedBlock(const void* block) const;

	// not allowed

	JMMArrayTable(const JMMArrayTable& source);
	const JMMArrayTable& operator=(const JMMArrayTable& source);
};

#endif
