/******************************************************************************
 RecordList.cpp

	BASE CLASS = JContainer

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "RecordList.h"
#include "Record.h"
#include <jx-af/jcore/jAssert.h>

// JBroadcaster messages

const JUtf8Byte* RecordList::kListChanged      = "ListChanged::RecordList";
const JUtf8Byte* RecordList::kPrepareForUpdate = "PrepareForUpdate::RecordList";

/******************************************************************************
 Constructor

 *****************************************************************************/

RecordList::RecordList()
	:
	itsSortColumn(kRecordState)
{
	itsRecords = jnew JPtrArray<Record>(JPtrArrayT::kDeleteAll);
	assert(itsRecords != nullptr);
	itsRecords->SetCompareFunction(Record::CompareState);
	InstallCollection(itsRecords);

	itsAlphaRecords = jnew JPtrArray<Record>(JPtrArrayT::kForgetAll);
	assert( itsAlphaRecords != nullptr );
	itsAlphaRecords->SetCompareFunction(Record::CompareFileNames);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

RecordList::~RecordList()
{
	jdelete itsRecords;
	jdelete itsAlphaRecords;
}

/******************************************************************************
 AddRecord

 ******************************************************************************/

void
RecordList::AddRecord
	(
	Record* record
	)
{
	itsRecords->InsertSorted(record);
	itsAlphaRecords->InsertSorted(record);
}

/******************************************************************************
 ClosestMatch

 ******************************************************************************/

bool
RecordList::ClosestMatch
	(
	const JString&	prefix,
	Record**		record
	)
	const
{
	Record target(prefix);
	bool found;
	JIndex i = itsAlphaRecords->SearchSortedOTI(&target, JListT::kFirstMatch, &found);
	if (i > itsAlphaRecords->GetItemCount())		// insert beyond end of list
	{
		i = itsAlphaRecords->GetItemCount();
	}

	if (i > 0)
	{
		*record = itsAlphaRecords->GetItem(i);
		return true;
	}
	else
	{
		*record = nullptr;
		return false;
	}
}

/******************************************************************************
 SetSortColumn

 ******************************************************************************/

void
RecordList::SetSortColumn
	(
	const JIndex index
	)
{
	bool changed = false;

	if (index == kRecordState)
	{
		itsRecords->SetCompareFunction(Record::CompareState);
		itsRecords->SetSortOrder(JListT::kSortAscending);
		itsSortColumn = kRecordState;
		changed       = true;
	}
	else if (index == kRecordFile || index == kRecordLine)
	{
		itsRecords->SetCompareFunction(Record::CompareFileNames);
		itsRecords->SetSortOrder(JListT::kSortAscending);
		itsSortColumn = kRecordFile;
		changed       = true;
	}
	else if (index == kRecordSize)
	{
		itsRecords->SetCompareFunction(Record::CompareSizes);
		itsRecords->SetSortOrder(JListT::kSortDescending);
		itsSortColumn = kRecordSize;
		changed       = true;
	}
	else if (index == kRecordData)
	{
		itsRecords->SetCompareFunction(Record::CompareData);
		itsRecords->SetSortOrder(JListT::kSortAscending);
		itsSortColumn = kRecordData;
		changed       = true;
	}

	if (changed)
	{
		Broadcast(PrepareForUpdate());
		itsRecords->Sort();
		Broadcast(ListChanged());
	}
}
