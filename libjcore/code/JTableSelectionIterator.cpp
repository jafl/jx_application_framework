/******************************************************************************
 JTableSelectionIterator.cpp

	This class provides a simple, error-resistant way to retrieve the
	coordinates of the cells that are selected and can iterate over the
	table either by row or by column.

	The cursor values run from one to row/col count.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#include "JTableSelectionIterator.h"
#include "JTableSelection.h"
#include "JMinMax.h"
#include "jAssert.h"

/******************************************************************************
 Constructor

	We can't overload the constructor to provide one that takes a JPoint
	because then the compiler can't tell which one to call because of all
	the default values.

 ******************************************************************************/

JTableSelectionIterator::JTableSelectionIterator
	(
	const JTableSelection*	tableSelection,
	const Direction			d,
	const Position			start,
	const JIndex			row,
	const JIndex			col
	)
{
	itsTableSelection = tableSelection;
	itsDirection      = d;

	MoveTo(start, row, col);

	ListenTo(itsTableSelection);
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JTableSelectionIterator::JTableSelectionIterator
	(
	const JTableSelectionIterator& source
	)
{
	itsTableSelection = source.itsTableSelection;
	itsDirection      = source.itsDirection;
	itsCursor         = source.itsCursor;
	itsAtEndFlag      = source.itsAtEndFlag;

	ListenTo(itsTableSelection);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTableSelectionIterator::~JTableSelectionIterator()
{
}

/******************************************************************************
 Prev (virtual)

	Return true if there is a previous selected cell, fetching the
	coordinates of the cell and decrementing the iterator position.
	Otherwise return false without fetching or decrementing.

 ******************************************************************************/

bool
JTableSelectionIterator::Prev
	(
	JIndex* row, 
	JIndex* col
	)
{
	JPoint cell;
	if (Prev(&cell))
	{
		*row = cell.y;
		*col = cell.x;
		return true;
	}
	else
	{
		*row = *col = 0;
		return false;
	}
}

bool
JTableSelectionIterator::Prev
	(
	JPoint* cell
	)
{
	if (AtBeginning())
	{
		*cell = JPoint(0,0);
		return false;
	}

	while (PrevCell())
	{
		if (itsTableSelection->IsSelected(itsCursor))
		{
			*cell = itsCursor;
			return true;
		}
	}

	*cell = JPoint(0,0);
	return false;
}

/******************************************************************************
 PrevCell (private)

 ******************************************************************************/

bool
JTableSelectionIterator::PrevCell()
{
	const JSize rowCount = itsTableSelection->GetRowCount();
	const JSize colCount = itsTableSelection->GetColCount();

	if (rowCount == 0 || colCount == 0)
	{
		return false;
	}

	else if (itsAtEndFlag)
	{
		// we simply clear the flag
		assert( (JIndex) itsCursor.x == colCount &&
				(JIndex) itsCursor.y == rowCount );
	}

	// check this after itsAtEndFlag to handle case where table is 1x1

	else if (itsCursor.y <= 1 && itsCursor.x <= 1)
	{
		return false;
	}

	else if (itsDirection == kIterateByCol)
	{
		if (itsCursor.y <= 1)
		{
			itsCursor.y = rowCount;
			itsCursor.x--;
		}
		else
		{
			itsCursor.y--;
		}
	}

	else	// itsDirection == kIterateByRow
	{
		if (itsCursor.x <= 1)
		{
			itsCursor.x = colCount;
			itsCursor.y--;
		}
		else
		{
			itsCursor.x--;
		}
	}

	itsAtEndFlag = false;
	return true;
}

/******************************************************************************
 Next (virtual)

	Return true if there is a next selected cell, fetching the
	coordinates of the cell and incrementing the iterator position.
	Otherwise return false without fetching or incrementing.

 ******************************************************************************/

bool
JTableSelectionIterator::Next
	(
	JIndex* row, 
	JIndex* col
	)
{
	JPoint cell;
	if (Next(&cell))
	{
		*row = cell.y;
		*col = cell.x;
		return true;
	}
	else
	{
		*row = *col = 0;
		return false;
	}
}

bool
JTableSelectionIterator::Next
	(
	JPoint* cell
	)
{
	while (!AtEnd())
	{
		*cell = itsCursor;
		NextCell();
		if (itsTableSelection->IsSelected(*cell))
		{
			return true;
		}
	}

	*cell = JPoint(0,0);
	return false;
}

/******************************************************************************
 NextCell (private)

 ******************************************************************************/

bool
JTableSelectionIterator::NextCell()
{
	JSize rowCount = itsTableSelection->GetRowCount();
	JSize colCount = itsTableSelection->GetColCount();

	if (rowCount == 0 || colCount == 0 ||
		((JIndex) itsCursor.y >= rowCount &&
		 (JIndex) itsCursor.x >= colCount))
	{
		itsAtEndFlag = true;
		return false;
	}

	else if (itsDirection == kIterateByCol)
	{
		if ((JIndex) itsCursor.y >= rowCount)
		{
			itsCursor.y = 1;
			itsCursor.x++;
		}
		else
		{
			itsCursor.y++;
		}
	}

	else	// itsDirection == kIterateByRow
	{
		if ((JIndex) itsCursor.x >= colCount)
		{
			itsCursor.x = 1;
			itsCursor.y++;
		}
		else
		{
			itsCursor.x++;
		}
	}

	return true;
}

/******************************************************************************
 MoveTo

 ******************************************************************************/

void
JTableSelectionIterator::MoveTo
	(
	const Position	newPosition,
	const JPoint&	cell
	)
{
	if (itsTableSelection == nullptr)
	{
		return;
	}

	const JSize rowCount = itsTableSelection->GetRowCount();
	const JSize colCount = itsTableSelection->GetColCount();

	itsAtEndFlag = false;

	if (newPosition == kStartAtBeginning)
	{
		itsCursor.x = 1;
		itsCursor.y = 1;
	}

	else if (newPosition == kStartAtEnd)
	{
		itsCursor.x  = JMax((JSize) 1, colCount);
		itsCursor.y  = JMax((JSize) 1, rowCount);
		itsAtEndFlag = true;
	}

	else if (newPosition == kStartBefore)
	{
		assert( itsTableSelection->CellValid(cell) );
		itsCursor = cell;
	}

	else
	{
		assert( newPosition == kStartAfter );
		assert( itsTableSelection->CellValid(cell) );
		itsCursor = cell;
		NextCell();
	}
}

/******************************************************************************
 AtBeginning

	Return true if iterator is positioned at the top left of the table
	or if the table has been deleted.

 ******************************************************************************/

bool
JTableSelectionIterator::AtBeginning()
	const
{
	return (!itsAtEndFlag && itsCursor.x <= 1 && itsCursor.y <= 1) ||
			itsTableSelection == nullptr ||
			itsTableSelection->GetRowCount() == 0 ||
			itsTableSelection->GetColCount() == 0;
}

/******************************************************************************
 AtEnd

	Return true if iterator is positioned at the bottom right of the table
	or if the table has been deleted.

 ******************************************************************************/

bool
JTableSelectionIterator::AtEnd()
	const
{
	return (itsAtEndFlag ||
			itsTableSelection == nullptr ||
			itsTableSelection->GetRowCount() == 0 ||
			itsTableSelection->GetColCount() == 0);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JTableSelectionIterator::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender != const_cast<JTableSelection*>(itsTableSelection))
	{
		return;
	}

	const JSize rowCount = itsTableSelection->GetRowCount();
	const JSize colCount = itsTableSelection->GetColCount();

	// rows changed

	if (message.Is(JTableData::kRowsInserted))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowsInserted*>(&message);
		assert( info != nullptr );
		info->AdjustCell(&itsCursor);
	}

	else if (message.Is(JTableData::kRowDuplicated))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowDuplicated*>(&message);
		assert( info != nullptr );
		info->AdjustCell(&itsCursor);
	}

	else if (message.Is(JTableData::kRowsRemoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowsRemoved*>(&message);
		assert( info != nullptr );
		JPoint newCursor = itsCursor;
		if (info->AdjustCell(&newCursor))
		{
			itsCursor = newCursor;
		}
		else if (itsDirection == kIterateByRow &&
				 (JIndex) itsCursor.y <= rowCount)
		{
			itsCursor.x = 1;
		}
		else if (itsDirection == kIterateByRow)
		{
			MoveTo(kStartAtEnd, 0,0);
		}
		else if (itsDirection == kIterateByCol &&
				 (JIndex) itsCursor.y > rowCount)
		{
			itsCursor.y = JMax((JSize) 1, rowCount);
			NextCell();
		}
	}

	// columns changed

	else if (message.Is(JTableData::kColsInserted))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColsInserted*>(&message);
		assert( info != nullptr );
		info->AdjustCell(&itsCursor);
	}

	else if (message.Is( JTableData::kColDuplicated))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColDuplicated*>(&message);
		assert( info != nullptr );
		info->AdjustCell(&itsCursor);
	}

	else if (message.Is(JTableData::kColsRemoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColsRemoved*>(&message);
		assert( info != nullptr );
		JPoint newCursor = itsCursor;
		if (info->AdjustCell(&newCursor))
		{
			itsCursor = newCursor;
		}
		else if (itsDirection == kIterateByCol &&
				 (JIndex) itsCursor.x <= colCount)
		{
			itsCursor.y = 1;
		}
		else if (itsDirection == kIterateByCol)
		{
			MoveTo(kStartAtEnd, 0,0);
		}
		else if (itsDirection == kIterateByRow &&
				 (JIndex) itsCursor.x > colCount)
		{
			itsCursor.x = JMax((JSize) 1, colCount);
			NextCell();
		}
	}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JTableSelectionIterator::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == const_cast<JTableSelection*>(itsTableSelection))
	{
		itsTableSelection = nullptr;
		itsCursor         = JPoint(1,1);
	}
}
