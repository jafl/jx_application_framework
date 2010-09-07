/******************************************************************************
 MDRecordList.cc

	BASE CLASS = JContainer

	Copyright © 2010 by John Lindal.  All rights reserved.

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
	itsRecords = new JPtrArray<MDRecord>(JPtrArrayT::kDeleteAll);
	assert(itsRecords != NULL);
	itsRecords->SetCompareFunction(MDRecord::CompareState);
	InstallOrderedSet(itsRecords);

	itsAlphaRecords = new JPtrArray<MDRecord>(JPtrArrayT::kForgetAll);
	assert( itsAlphaRecords != NULL );
	itsAlphaRecords->SetCompareFunction(MDRecord::CompareFileName);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDRecordList::~MDRecordList()
{
	delete itsRecords;
	delete itsAlphaRecords;
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
	JIndex i = itsAlphaRecords->SearchSorted1(&target, JOrderedSetT::kFirstMatch, &found);
	if (i > itsAlphaRecords->GetElementCount())		// insert beyond end of list
		{
		i = itsAlphaRecords->GetElementCount();
		}

	if (i > 0)
		{
		*record = itsAlphaRecords->NthElement(i);
		return kJTrue;
		}
	else
		{
		*record = NULL;
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
		itsRecords->SetSortOrder(JOrderedSetT::kSortAscending);
		itsSortColumn = kRecordState;
		changed       = kJTrue;
		}
	else if (index == kRecordFile || index == kRecordLine)
		{
		itsRecords->SetCompareFunction(MDRecord::CompareFileName);
		itsRecords->SetSortOrder(JOrderedSetT::kSortAscending);
		itsSortColumn = kRecordFile;
		changed       = kJTrue;
		}
	else if (index == kRecordSize)
		{
		itsRecords->SetCompareFunction(MDRecord::CompareSize);
		itsRecords->SetSortOrder(JOrderedSetT::kSortDescending);
		itsSortColumn = kRecordSize;
		changed       = kJTrue;
		}
	else if (index == kRecordData)
		{
		itsRecords->SetCompareFunction(MDRecord::CompareData);
		itsRecords->SetSortOrder(JOrderedSetT::kSortAscending);
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
