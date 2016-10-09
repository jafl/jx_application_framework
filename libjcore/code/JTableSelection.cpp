/******************************************************************************
 JTableSelection.cpp

	Class for storing which cells are selected in a JTable.

	BASE CLASS = JAuxTableData<JBoolean>

	Copyright (C) 1997 by John Lindal & Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <JTableSelection.h>
#include <JTable.h>
#include <JMinMax.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JTableSelection::JTableSelection
	(
	JTable* table
	)
	:
	JAuxTableData<JBoolean>(table, kJFalse)
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JTableSelection::JTableSelection
	(
	JTable*					table,
	const JTableSelection&	source
	)
	:
	JAuxTableData<JBoolean>(table, source)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JTableSelection::~JTableSelection()
{
}

/******************************************************************************
 ExtendSelection

	Undoes the latest extend operation, sets the jnew boat cell, and
	selects the cells between boat and anchor.

 ******************************************************************************/

JBoolean
JTableSelection::ExtendSelection
	(
	const JPoint& newBoat
	)
{
	if (newBoat == itsBoat)
		{
		return kJTrue;
		}
	else if (UndoSelection())
		{
		itsBoat = newBoat;
		SelectRect(itsBoat, itsAnchor, kJTrue);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 UndoSelection (private)

	Undoes the latest extend operation.

 ******************************************************************************/

JBoolean
JTableSelection::UndoSelection()
{
	if (OKToExtendSelection())
		{
		SelectRect(itsBoat, itsAnchor, kJFalse);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 Receive (virtual protected)

	Update our boat and anchor cells.

	When a row or column is moved, there is nothing to do but reselect
	the same area in the jnew arrangement.

 ******************************************************************************/

void
JTableSelection::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JTable* table = GetTable();

	JBoolean reselect = kJFalse;
	if (sender == table && message.Is(JTable::kPrepareForTableDataMessage))
		{
		itsReselectAfterChangeFlag = UndoSelection();
		}

	// let JAuxTableData update the data first

	JAuxTableData<JBoolean>::Receive(sender, message);

	// rows changed

	if (sender == table && message.Is(JTableData::kRowsInserted))
		{
		const JTableData::RowsInserted* info =
			dynamic_cast<const JTableData::RowsInserted*>(&message);
		assert( info != NULL );
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		reselect = kJTrue;
		}

	else if (sender == table && message.Is(JTableData::kRowDuplicated))
		{
		const JTableData::RowDuplicated* info =
			dynamic_cast<const JTableData::RowDuplicated*>(&message);
		assert( info != NULL );
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		reselect = kJTrue;
		}

	else if (sender == table && message.Is(JTableData::kRowsRemoved))
		{
		const JTableData::RowsRemoved* info =
			dynamic_cast<const JTableData::RowsRemoved*>(&message);
		assert( info != NULL );
		const JPoint origBoat   = itsBoat;
		const JPoint origAnchor = itsAnchor;
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		AdjustIndexAfterRemove(origBoat.y, origAnchor.y,
							   info->GetFirstIndex(), GetRowCount(),
							   &(itsBoat.y), &(itsAnchor.y));
		reselect = kJTrue;
		}

	// columns changed

	else if (sender == table && message.Is(JTableData::kColsInserted))
		{
		const JTableData::ColsInserted* info =
			dynamic_cast<const JTableData::ColsInserted*>(&message);
		assert( info != NULL );
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		reselect = kJTrue;
		}

	else if (sender == table && message.Is(JTableData::kColDuplicated))
		{
		const JTableData::ColDuplicated* info =
			dynamic_cast<const JTableData::ColDuplicated*>(&message);
		assert( info != NULL );
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		reselect = kJTrue;
		}

	else if (sender == table && message.Is(JTableData::kColsRemoved))
		{
		const JTableData::ColsRemoved* info =
			dynamic_cast<const JTableData::ColsRemoved*>(&message);
		assert( info != NULL );
		const JPoint origBoat   = itsBoat;
		const JPoint origAnchor = itsAnchor;
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		AdjustIndexAfterRemove(origBoat.x, origAnchor.x,
							   info->GetFirstIndex(), GetColCount(),
							   &(itsBoat.x), &(itsAnchor.x));
		reselect = kJTrue;
		}

	// elements changed

	else if (sender == table && message.Is(JTableData::kRectChanged))
		{
		reselect = kJTrue;
		}

	// everything changed

	else if (sender == table && message.Is(JTable::kTableDataChanged))
		{
		ClearSelection();
		}

	// select jnew area

	if (reselect && itsReselectAfterChangeFlag && OKToExtendSelection())
		{
		SelectRect(itsBoat, itsAnchor, kJTrue);
		}
}

/******************************************************************************
 AdjustIndexAfterRemove (private)

 ******************************************************************************/

void
JTableSelection::AdjustIndexAfterRemove
	(
	const JIndex	origBoat,
	const JIndex	origAnchor,
	const JIndex	firstIndex,
	const JIndex	maxIndex,
	JCoordinate*	boat,
	JCoordinate*	anchor
	)
	const
{
	if (*boat == 0 && *anchor > 0 && origAnchor < origBoat)
		{
		*boat = JMin(firstIndex-1, maxIndex);
		}
	else if (*boat == 0 && *anchor > 0)
		{
		*boat = JMin(firstIndex, maxIndex);
		}
	else if (*anchor == 0 && *boat > 0 && origBoat < origAnchor)
		{
		*anchor = JMin(firstIndex-1, maxIndex);
		}
	else if (*anchor == 0 && *boat > 0)
		{
		*anchor = JMin(firstIndex, maxIndex);
		}
}

/******************************************************************************
 GetSingleSelectedCell

 ******************************************************************************/

JBoolean
JTableSelection::GetSingleSelectedCell
	(
	JPoint* cell
	)
	const
{
	JPoint cell1, cell2;
	if (GetFirstSelectedCell(&cell1) &&
		GetLastSelectedCell(&cell2) &&
		cell1 == cell2)
		{
		*cell = cell1;
		return kJTrue;
		}
	else
		{
		cell->Set(0,0);
		return kJFalse;
		}
}

/******************************************************************************
 GetFirstSelectedCell

	Return the first selected cell, starting from the upper left and going in
	the specified direction.

 ******************************************************************************/

JBoolean
JTableSelection::GetFirstSelectedCell
	(
	JPoint*										cell,
	const JTableSelectionIterator::Direction	d
	)
	const
{
	JTableSelectionIterator iter(this, d, kJIteratorStartAtBeginning);
	return iter.Next(cell);
}

/******************************************************************************
 GetLastSelectedCell

	Return the first selected cell, starting from the lower right and going in
	the specified direction.

 ******************************************************************************/

JBoolean
JTableSelection::GetLastSelectedCell
	(
	JPoint*										cell,
	const JTableSelectionIterator::Direction	d
	)
	const
{
	JTableSelectionIterator iter(this, d, kJIteratorStartAtEnd);
	return iter.Prev(cell);
}

/******************************************************************************
 InvertSelection (static private)

	Not inline because we need a pointer to it.

 ******************************************************************************/

JBoolean
JTableSelection::InvertSelection
	(
	const JBoolean& b
	)
{
	return !b;
}
