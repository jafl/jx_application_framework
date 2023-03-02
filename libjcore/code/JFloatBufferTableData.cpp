/******************************************************************************
 JFloatBufferTableData.cpp

	Class to buffer a table of numbers as JStrings so drawing the table
	will be faster.

	BASE CLASS = JStringTableData

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JFloatBufferTableData.h"
#include "JFloatTableData.h"
#include "JString.h"
#include "jAssert.h"

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
	itsFloatData(floatData),
	itsPrecision(precision)
{
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
		const auto* info =
			dynamic_cast<const JTableData::RectChanged*>(&message);
		assert( info != nullptr );
		UpdateRect(info->GetRect());
	}

	// rows changed

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kRowsInserted))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowsInserted*>(&message);
		assert( info != nullptr );
		InsertRows(info->GetFirstIndex(), info->GetCount());
		UpdateRows(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kRowDuplicated))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowDuplicated*>(&message);
		assert( info != nullptr );
		DuplicateRow(info->GetOrigIndex(), info->GetNewIndex());
	}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kRowsRemoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowsRemoved*>(&message);
		assert( info != nullptr );
		RemoveNextRows(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kRowMoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowMoved*>(&message);
		assert( info != nullptr );
		MoveRow(info->GetOrigIndex(), info->GetNewIndex());
	}

	// columns changed

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kColsInserted))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColsInserted*>(&message);
		assert( info != nullptr );
		InsertCols(info->GetFirstIndex(), info->GetCount());
		UpdateCols(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kColDuplicated))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColDuplicated*>(&message);
		assert( info != nullptr );
		DuplicateCol(info->GetOrigIndex(), info->GetNewIndex());
	}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kColsRemoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColsRemoved*>(&message);
		assert( info != nullptr );
		RemoveNextCols(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == const_cast<JFloatTableData*>(itsFloatData) &&
			 message.Is(JTableData::kColMoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColMoved*>(&message);
		assert( info != nullptr );
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
 UpdateRows (private)

 ******************************************************************************/

inline void
JFloatBufferTableData::UpdateRows
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	UpdateRect(JRect(firstIndex, 1, firstIndex+count, GetColCount()+1));
}

/******************************************************************************
 UpdateCols (private)

 ******************************************************************************/

inline void
JFloatBufferTableData::UpdateCols
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	UpdateRect(JRect(1, firstIndex, GetRowCount()+1, firstIndex+count));
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
