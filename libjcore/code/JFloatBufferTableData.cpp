/******************************************************************************
 JFloatBufferTableData.cpp

	Class to buffer a table of numbers as JStrings so drawing the table
	will be faster.

	BASE CLASS = JStringTableData

	Copyright © 1996 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JFloatBufferTableData.h>
#include <JFloatTableData.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JFloatBufferTableData::JFloatBufferTableData
	(
	const JFloatTableData*	floatData,
	const int				precision
	)
	:
	JStringTableData(),
	itsPrecision(precision)
{
	itsFloatData = floatData;
	ListenTo(itsFloatData);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JFloatBufferTableData::~JFloatBufferTableData()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JFloatBufferTableData::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// element data changed

	if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
		message.Is(JTableData::kRectChanged))
		{
		const JTableData::RectChanged* info =
			dynamic_cast(const JTableData::RectChanged*, &message);
		assert( info != NULL );
		UpdateRect(info->GetRect());
		}

	// rows changed

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kRowsInserted))
		{
		const JTableData::RowsInserted* info =
			dynamic_cast(const JTableData::RowsInserted*, &message);
		assert( info != NULL );
		InsertRows(info->GetFirstIndex(), info->GetCount());
		UpdateRows(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kRowDuplicated))
		{
		const JTableData::RowDuplicated* info =
			dynamic_cast(const JTableData::RowDuplicated*, &message);
		assert( info != NULL );
		DuplicateRow(info->GetOrigIndex(), info->GetNewIndex());
		}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kRowsRemoved))
		{
		const JTableData::RowsRemoved* info =
			dynamic_cast(const JTableData::RowsRemoved*, &message);
		assert( info != NULL );
		RemoveNextRows(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kRowMoved))
		{
		const JTableData::RowMoved* info =
			dynamic_cast(const JTableData::RowMoved*, &message);
		assert( info != NULL );
		MoveRow(info->GetOrigIndex(), info->GetNewIndex());
		}

	// columns changed

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kColsInserted))
		{
		const JTableData::ColsInserted* info =
			dynamic_cast(const JTableData::ColsInserted*, &message);
		assert( info != NULL );
		InsertCols(info->GetFirstIndex(), info->GetCount());
		UpdateCols(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kColDuplicated))
		{
		const JTableData::ColDuplicated* info =
			dynamic_cast(const JTableData::ColDuplicated*, &message);
		assert( info != NULL );
		DuplicateCol(info->GetOrigIndex(), info->GetNewIndex());
		}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kColsRemoved))
		{
		const JTableData::ColsRemoved* info =
			dynamic_cast(const JTableData::ColsRemoved*, &message);
		assert( info != NULL );
		RemoveNextCols(info->GetFirstIndex(), info->GetCount());
		}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kColMoved))
		{
		const JTableData::ColMoved* info =
			dynamic_cast(const JTableData::ColMoved*, &message);
		assert( info != NULL );
		MoveCol(info->GetOrigIndex(), info->GetNewIndex());
		}

	// something else

	else
		{
		JStringTableData::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateRect (private)

 ******************************************************************************/

void
JFloatBufferTableData::UpdateRect
	(
	const JRect& r
	)
{
	for (JIndex x=r.left; x < (JIndex) r.right; x++)
		{
		for (JIndex y=r.top; y < (JIndex) r.bottom; y++)
			{
			UpdateCell(JPoint(x,y));
			}
		}
}

/******************************************************************************
 UpdateCell (private)

 ******************************************************************************/

void
JFloatBufferTableData::UpdateCell
	(
	const JPoint& cell
	)
{
	const JFloat value = itsFloatData->GetElement(cell);
	const JString valueStr(value, itsPrecision);
	SetElement(cell, valueStr);
}
