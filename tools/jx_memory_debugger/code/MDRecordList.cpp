/******************************************************************************
 MDRecordList.cpp

	BASE CLASS = JContainer

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDRecordList.h"
#include "MDRecord.h"
#include <jAssert.h>

// JBroadcaster messages

const JUtf8Byte* MDRecordList::kListChanged      = "ListChanged::MDRecordList";
const JUtf8Byte* MDRecordList::kPrepareForUpdate = "PrepareForUpdate::MDRecordList";

/******************************************************************************
 Constructor

 *****************************************************************************/

MDRecordList::MDRecordList()
	:
	itsSortColumn(kRecordState)
{
	itsRecords = jnew JPtrArray<MDRecord>(JPtrArrayT::kDeleteAll);
	assert(itsRecords != nullptr);
	itsRecords->SetCompareFunction(MDRecord::CompareState);
	InstallCollection(itsRecords);

	itsAlphaRecords = jnew JPtrArray<MDRecord>(JPtrArrayT::kForgetAll);
	assert( itsAlphaRecords != nullptr );
	itsAlphaRecords->SetCompareFunction(MDRecord::CompareFileName);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDRecordList::~MDRecordList()
{
	jdelete itsRecords;
	jdelete itsAlphaRecords;
}

/******************************************************************************
 AddRecord

 ******************************************************************************/

void
MDRecordList::AddRecord
	(
	MDRecord* record
	)
{
	itsRecords->InsertSorted(record);
	itsAlphaRecords->InsertSorted(record);
}

/******************************************************************************
 ClosestMatch

 ******************************************************************************/

bool
MDRecordList::ClosestMatch
	(
	const JString&	prefix,
	MDRecord**		record
	)
	const
{
	MDRecord target(prefix);
	bool found;
	JIndex i = itsAlphaRecords->SearchSorted1(&target, JListT::kFirstMatch, &found);
	if (i > itsAlphaRecords->GetElementCount())		// insert beyond end of list
	{
		i = itsAlphaRecords->GetElementCount();
	}

	if (i > 0)
	{
		*record = itsAlphaRecords->GetElement(i);
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
MDRecordList::SetSortColumn
	(
	const JIndex index
	)
{
	bool changed = false;

	if (index == kRecordState)
	{
		itsRecords->SetCompareFunction(MDRecord::CompareState);
		itsRecords->SetSortOrder(JListT::kSortAscending);
		itsSortColumn = kRecordState;
		changed       = true;
	}
	else if (index == kRecordFile || index == kRecordLine)
	{
		itsRecords->SetCompareFunction(MDRecord::CompareFileName);
		itsRecords->SetSortOrder(JListT::kSortAscending);
		itsSortColumn = kRecordFile;
		changed       = true;
	}
	else if (index == kRecordSize)
	{
		itsRecords->SetCompareFunction(MDRecord::CompareSize);
		itsRecords->SetSortOrder(JListT::kSortDescending);
		itsSortColumn = kRecordSize;
		changed       = true;
	}
	else if (index == kRecordData)
	{
		itsRecords->SetCompareFunction(MDRecord::CompareData);
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
