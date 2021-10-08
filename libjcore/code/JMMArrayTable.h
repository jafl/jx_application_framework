#ifndef _H_JMMArrayTable
#define _H_JMMArrayTable

/******************************************************************************
 JMMArrayTable.h

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "jx-af/jcore/JMMTable.h"
#include "jx-af/jcore/JArray.h"

class JMMArrayTable : public JMMTable
{
public:

	JMMArrayTable(JMemoryManager* manager, const bool recordDelete);
	~JMMArrayTable();

	JSize GetAllocatedCount() const override;
	JSize GetAllocatedBytes() const override;
	JSize GetDeletedCount() const override;
	JSize GetTotalCount() const override;

	void PrintAllocated(const bool printInternal = false) const override;
	void StreamAllocatedForDebug(std::ostream& output, const JMemoryManager::RecordFilter& filter) const override;
	void StreamAllocationSizeHistogram(std::ostream& output) const override;

protected:

	void _AddNewRecord(const JMMRecord& record,
					   const bool checkDoubleAllocation) override;

	bool _SetRecordDeleted(JMMRecord* record, const void* block,
						   const JUtf8Byte* file, const JUInt32 line,
						   const bool isArray) override;

private:

	JArray<JMMRecord>*	itsAllocatedTable;
	JSize				itsAllocatedBytes;
	JArray<JMMRecord>*	itsDeletedTable;
	JSize				itsDeletedCount;

private:

	JSize FindAllocatedBlock(const void* block) const;
	JSize FindDeletedBlock(const void* block) const;
};

#endif
