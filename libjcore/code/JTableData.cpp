/******************************************************************************
 JTableData.cpp

	Abstract class for data in some kind of 2D arrangement.  The common
	ground for TableData objects is that the data is accessed by giving
	the row and column indices.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JTableData.h"
#include "JListUtil.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JTableData::JTableData()
{
	itsRowCount = itsColCount = 0;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JTableData::JTableData
	(
	const JTableData& source
	)
	:
	JBroadcaster(source)
{
	itsRowCount = source.itsRowCount;
	itsColCount = source.itsColCount;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTableData::~JTableData()
{
}

/******************************************************************************
 Assignment operator

 ******************************************************************************/

const JTableData&
JTableData::operator=
	(
	const JTableData& source
	)
{
	if (this == &source)
		{
		return *this;
		}

	JBroadcaster::operator=(source);

	itsRowCount = source.itsRowCount;
	itsColCount = source.itsColCount;

	return *this;
}

/******************************************************************************
 RowsDeleted (protected)

	We use "Deleted" to avoid conflicting with RowsRemoved message.

	Not inline because it uses assert

 ******************************************************************************/

void
JTableData::RowsDeleted
	(
	const JSize count
	)
{
	assert( itsRowCount >= count );
	itsRowCount -= count;
}

/******************************************************************************
 ColsDeleted (protected)

	We use "Deleted" to avoid conflicting with ColsRemoved message.

	Not inline because it uses assert

 ******************************************************************************/

void
JTableData::ColsDeleted
	(
	const JSize count
	)
{
	assert( itsColCount >= count );
	itsColCount -= count;
}

/******************************************************************************
 JBroadcaster messages

 ******************************************************************************/

// message types

const JUtf8Byte* JTableData::kRowsInserted  = "RowsInserted::JTableData";
const JUtf8Byte* JTableData::kRowDuplicated = "RowDuplicated::JTableData";
const JUtf8Byte* JTableData::kRowsRemoved   = "RowsRemoved::JTableData";
const JUtf8Byte* JTableData::kRowMoved      = "RowMoved::JTableData";

const JUtf8Byte* JTableData::kColsInserted  = "ColsInserted::JTableData";
const JUtf8Byte* JTableData::kColDuplicated = "ColDuplicated::JTableData";
const JUtf8Byte* JTableData::kColsRemoved   = "ColsRemoved::JTableData";
const JUtf8Byte* JTableData::kColMoved      = "ColMoved::JTableData";

const JUtf8Byte* JTableData::kRectChanged   = "RectChanged::JTableData";

/******************************************************************************
 AdjustIndex

 ******************************************************************************/

void
JTableData::RowsInserted::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(GetFirstIndex(), GetCount(), index);
}

JBoolean
JTableData::RowsRemoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	return JAdjustIndexAfterRemove(GetFirstIndex(), GetCount(), index);
}

void
JTableData::ColsInserted::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(GetFirstIndex(), GetCount(), index);
}

JBoolean
JTableData::ColsRemoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	return JAdjustIndexAfterRemove(GetFirstIndex(), GetCount(), index);
}

void
JTableData::RowColDupd::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(itsNewIndex, 1, index);
}

void
JTableData::RowColMoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterMove(itsOrigIndex, itsNewIndex, index);
}
