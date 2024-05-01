/******************************************************************************
 JBufferTableData.cpp

	Class to buffer a table of values as JStrings so drawing the table
	will be faster.

	BASE CLASS = JStringTableData

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "JBufferTableData.h"
#include "JFloatTableData.h"
#include "JString.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

	Derived class must call UpdateRect()!

 ******************************************************************************/

JBufferTableData::JBufferTableData
	(
	const JTableData* data
	)
	:
	itsData(data)
{
	AppendRows(data->GetRowCount());
	AppendCols(data->GetColCount());

	ListenTo(itsData);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JBufferTableData::~JBufferTableData()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JBufferTableData::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	auto* data = const_cast<JTableData*>(itsData);

	// element data changed

	if (sender == data && message.Is(JTableData::kRectChanged))
	{
		auto* info = dynamic_cast<const JTableData::RectChanged*>(&message);
		assert( info != nullptr );
		UpdateRect(info->GetRect());
	}

	// rows changed

	else if (sender == data && message.Is(JTableData::kRowsInserted))
	{
		auto* info = dynamic_cast<const JTableData::RowsInserted*>(&message);
		assert( info != nullptr );
		InsertRows(info->GetFirstIndex(), info->GetCount());
		UpdateRows(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == data && message.Is(JTableData::kRowDuplicated))
	{
		auto* info = dynamic_cast<const JTableData::RowDuplicated*>(&message);
		assert( info != nullptr );
		DuplicateRow(info->GetOrigIndex(), info->GetNewIndex());
	}

	else if (sender == data && message.Is(JTableData::kRowsRemoved))
	{
		auto* info = dynamic_cast<const JTableData::RowsRemoved*>(&message);
		assert( info != nullptr );
		RemoveNextRows(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == data && message.Is(JTableData::kRowMoved))
	{
		auto* info = dynamic_cast<const JTableData::RowMoved*>(&message);
		assert( info != nullptr );
		MoveRow(info->GetOrigIndex(), info->GetNewIndex());
	}

	// columns changed

	else if (sender == data && message.Is(JTableData::kColsInserted))
	{
		auto* info = dynamic_cast<const JTableData::ColsInserted*>(&message);
		assert( info != nullptr );
		InsertCols(info->GetFirstIndex(), info->GetCount());
		UpdateCols(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == data && message.Is(JTableData::kColDuplicated))
	{
		auto* info = dynamic_cast<const JTableData::ColDuplicated*>(&message);
		assert( info != nullptr );
		DuplicateCol(info->GetOrigIndex(), info->GetNewIndex());
	}

	else if (sender == data && message.Is(JTableData::kColsRemoved))
	{
		auto* info = dynamic_cast<const JTableData::ColsRemoved*>(&message);
		assert( info != nullptr );
		RemoveNextCols(info->GetFirstIndex(), info->GetCount());
	}

	else if (sender == data && message.Is(JTableData::kColMoved))
	{
		auto* info = dynamic_cast<const JTableData::ColMoved*>(&message);
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
 UpdateRect (protected)

 ******************************************************************************/

void
JBufferTableData::UpdateRect
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
 UpdateRows (protected)

 ******************************************************************************/

inline void
JBufferTableData::UpdateRows
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	UpdateRect(JRect(firstIndex, 1, firstIndex+count, GetColCount()+1));
}

/******************************************************************************
 UpdateCols (protected)

 ******************************************************************************/

inline void
JBufferTableData::UpdateCols
	(
	const JIndex	firstIndex,
	const JSize		count
	)
{
	UpdateRect(JRect(1, firstIndex, GetRowCount()+1, firstIndex+count));
}
