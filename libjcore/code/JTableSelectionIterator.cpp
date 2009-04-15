/******************************************************************************
 JTableSelectionIterator.cpp

	This class provides a simple, error-resistant way to retrieve the
	coordinates of the cells that are selected and can iterate over the
	table either by row or by column.

	The cursor values run from one to row/col count.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JTableSelectionIterator.h>
#include <JTableSelection.h>
#include <JMinMax.h>
#include <jAssert.h>

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
	const JIteratorPosition	start,
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

	Return kJTrue if there is a previous selected cell, fetching the
	coordinates of the cell and decrementing the iterator position.
	Otherwise return kJFalse without fetching or decrementing.

 ******************************************************************************/

JBoolean
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
		return kJTrue;
		}
	else
		{
		*row = *col = 0;
		return kJFalse;
		}
}

JBoolean
JTableSelectionIterator::Prev
	(
	JPoint* cell
	)
{
	if (AtBeginning())
		{
		*cell = JPoint(0,0);
		return kJFalse;
		}

	while (PrevCell())
		{
		if (itsTableSelection->IsSelected(itsCursor))
			{
			*cell = itsCursor;
			return kJTrue;
			}
		}

	*cell = JPoint(0,0);
	return kJFalse;
}

/******************************************************************************
 PrevCell (private)

 ******************************************************************************/

JBoolean
JTableSelectionIterator::PrevCell()
{
	const JSize rowCount = itsTableSelection->GetRowCount();
	const JSize colCount = itsTableSelection->GetColCount();

	if (rowCount == 0 || colCount == 0)
		{
		return kJFalse;
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
		return kJFalse;
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

	itsAtEndFlag = kJFalse;
	return kJTrue;
}

/******************************************************************************
 Next (virtual)

	Return kJTrue if there is a next selected cell, fetching the
	coordinates of the cell and incrementing the iterator position.
	Otherwise return kJFalse without fetching or incrementing.

 ******************************************************************************/

JBoolean
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
		return kJTrue;
		}
	else
		{
		*row = *col = 0;
		return kJFalse;
		}
}

JBoolean
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
			return kJTrue;
			}
		}

	*cell = JPoint(0,0);
	return kJFalse;
}

/******************************************************************************
 NextCell (private)

 ******************************************************************************/

JBoolean
JTableSelectionIterator::NextCell()
{
	JSize rowCount = itsTableSelection->GetRowCount();
	JSize colCount = itsTableSelection->GetColCount();

	if (rowCount == 0 || colCount == 0 ||
		((JIndex) itsCursor.y >= rowCount &&
		 (JIndex) itsCursor.x >= colCount))
		{
		itsAtEndFlag = kJTrue;
		return kJFalse;
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

	return kJTrue;
}

/******************************************************************************
 MoveTo

 ******************************************************************************/

void
JTableSelectionIterator::MoveTo
	(
	const JIteratorPosition	newPosition,
	const JPoint&			cell
	)
{
	if (itsTableSelection == NULL)
		{
		return;
		}

	const JSize rowCount = itsTableSelection->GetRowCount();
	const JSize colCount = itsTableSelection->GetColCount();

	itsAtEndFlag = kJFalse;

	if (newPosition == kJIteratorStartAtBeginning)
		{
		itsCursor.x = 1;
		itsCursor.y = 1;
		}

	else if (newPosition == kJIteratorStartAtEnd)
		{
		itsCursor.x  = JMax((JSize) 1, colCount);
		itsCursor.y  = JMax((JSize) 1, rowCount);
		itsAtEndFlag = kJTrue;
		}

	else if (newPosition == kJIteratorStartBefore)
		{
		assert( itsTableSelection->CellValid(cell) );
		itsCursor = cell;
		}

	else
		{
		assert( newPosition == kJIteratorStartAfter );
		assert( itsTableSelection->CellValid(cell) );
		itsCursor = cell;
		NextCell();
		}
}

/******************************************************************************
 AtBeginning

	Return kJTrue if iterator is positioned at the top left of the table
	or if the table has been deleted.

 ******************************************************************************/

JBoolean
JTableSelectionIterator::AtBeginning()
{
	return JI2B( (!itsAtEndFlag && itsCursor.x <= 1 && itsCursor.y <= 1) ||
				 itsTableSelection == NULL ||
				 itsTableSelection->GetRowCount() == 0 ||
				 itsTableSelection->GetColCount() == 0 );
}

/******************************************************************************
 AtEnd

	Return kJTrue if iterator is positioned at the bottom right of the table
	or if the table has been deleted.

 ******************************************************************************/

JBoolean
JTableSelectionIterator::AtEnd()
{
	return JI2B( itsAtEndFlag ||
				 itsTableSelection == NULL ||
				 itsTableSelection->GetRowCount() == 0 ||
				 itsTableSelection->GetColCount() == 0 );
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
		const JTableData::RowsInserted* info =
			dynamic_cast(const JTableData::RowsInserted*, &message);
		assert( info != NULL );
		info->AdjustCell(&itsCursor);
		}

	else if (message.Is(JTableData::kRowDuplicated))
		{
		const JTableData::RowDuplicated* info =
			dynamic_cast(const JTableData::RowDuplicated*, &message);
		assert( info != NULL );
		info->AdjustCell(&itsCursor);
		}

	else if (message.Is(JTableData::kRowsRemoved))
		{
		const JTableData::RowsRemoved* info =
			dynamic_cast(const JTableData::RowsRemoved*, &message);
		assert( info != NULL );
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
			MoveTo(kJIteratorStartAtEnd, 0,0);
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
		const JTableData::ColsInserted* info =
			dynamic_cast(const JTableData::ColsInserted*, &message);
		assert( info != NULL );
		info->AdjustCell(&itsCursor);
		}

	else if (message.Is( JTableData::kColDuplicated))
		{
		const JTableData::ColDuplicated* info =
			dynamic_cast(const JTableData::ColDuplicated*, &message);
		assert( info != NULL );
		info->AdjustCell(&itsCursor);
		}

	else if (message.Is(JTableData::kColsRemoved))
		{
		const JTableData::ColsRemoved* info =
			dynamic_cast(const JTableData::ColsRemoved*, &message);
		assert( info != NULL );
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
			MoveTo(kJIteratorStartAtEnd, 0,0);
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
		itsTableSelection = NULL;
		itsCursor         = JPoint(1,1);
		}
}
