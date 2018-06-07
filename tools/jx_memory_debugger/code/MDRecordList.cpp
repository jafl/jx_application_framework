/******************************************************************************
 MDRecordList.cc

	BASE CLASS = JContainer

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDRecordList.h"
#include "MDRecord.h"
#include <jAssert.h>

// JBroadcaster messages

const JCharacter* MDRecordList::kListChanged      = "ListChanged::MDRecordList";
const JCharacter* MDRecordList::kPrepareForUpdate = "PrepareForUpdate::MDRecordList";

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
	InstallOrderedSet(itsRecords);

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

JBoolean
MDRecordList::ClosestMatch
	(
	const JCharacter*	prefix,
	MDRecord**			record
	)
	const
{
	MDRecord target(prefix);
	JBoolean found;
	JIndex i = itsAlphaRecords->SearchSorted1(&target, JListT::kFirstMatch, &found);
	if (i > itsAlphaRecords->GetElementCount())		// insert beyond end of list
		{
		i = itsAlphaRecords->GetElementCount();
		}

	if (i > 0)
		{
		*record = itsAlphaRecords->GetElement(i);
		return kJTrue;
		}
	else
		{
		*record = nullptr;
		return kJFalse;
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
	JBoolean changed = kJFalse;

	if (index == kRecordState)
		{
		itsRecords->SetCompareFunction(MDRecord::CompareState);
		itsRecords->SetSortOrder(JListT::kSortAscending);
		itsSortColumn = kRecordState;
		changed       = kJTrue;
		}
	else if (index == kRecordFile || index == kRecordLine)
		{
		itsRecords->SetCompareFunction(MDRecord::CompareFileName);
		itsRecords->SetSortOrder(JListT::kSortAscending);
		itsSortColumn = kRecordFile;
		changed       = kJTrue;
		}
	else if (index == kRecordSize)
		{
		itsRecords->SetCompareFunction(MDRecord::CompareSize);
		itsRecords->SetSortOrder(JListT::kSortDescending);
		itsSortColumn = kRecordSize;
		changed       = kJTrue;
		}
	else if (index == kRecordData)
		{
		itsRecords->SetCompareFunction(MDRecord::CompareData);
		itsRecords->SetSortOrder(JListT::kSortAscending);
		itsSortColumn = kRecordData;
		changed       = kJTrue;
		}

	if (changed)
		{
		Broadcast(PrepareForUpdate());
		itsRecords->Sort();
		Broadcast(ListChanged());
		}
}
