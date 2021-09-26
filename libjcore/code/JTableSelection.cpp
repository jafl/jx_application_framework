/******************************************************************************
 JTableSelection.cpp

	Class for storing which cells are selected in a JTable.

	BASE CLASS = JAuxTableData<bool>

	Copyright (C) 1997 by John Lindal & Glenn Bach.

 ******************************************************************************/

#include "jx-af/jcore/JTableSelection.h"
#include "jx-af/jcore/JTable.h"
#include "jx-af/jcore/JMinMax.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JTableSelection::JTableSelection
	(
	JTable* table
	)
	:
	JAuxTableData<bool>(table, false)
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
	JAuxTableData<bool>(table, source)
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

	Undoes the latest extend operation, sets the new boat cell, and
	selects the cells between boat and anchor.

 ******************************************************************************/

bool
JTableSelection::ExtendSelection
	(
	const JPoint& newBoat
	)
{
	if (newBoat == itsBoat)
	{
		return true;
	}
	else if (UndoSelection())
	{
		itsBoat = newBoat;
		SelectRect(itsBoat, itsAnchor, true);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 UndoSelection (private)

	Undoes the latest extend operation.

 ******************************************************************************/

bool
JTableSelection::UndoSelection()
{
	if (OKToExtendSelection())
	{
		SelectRect(itsBoat, itsAnchor, false);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Receive (virtual protected)

	Update our boat and anchor cells.

	When a row or column is moved, there is nothing to do but reselect
	the same area in the new arrangement.

 ******************************************************************************/

void
JTableSelection::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JTable* table = GetTable();

	bool reselect = false;
	if (sender == table && message.Is(JTable::kPrepareForTableDataMessage))
	{
		itsReselectAfterChangeFlag = UndoSelection();
	}

	// let JAuxTableData update the data first

	JAuxTableData<bool>::Receive(sender, message);

	// rows changed

	if (sender == table && message.Is(JTableData::kRowsInserted))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowsInserted*>(&message);
		assert( info != nullptr );
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		reselect = true;
	}

	else if (sender == table && message.Is(JTableData::kRowDuplicated))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowDuplicated*>(&message);
		assert( info != nullptr );
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		reselect = true;
	}

	else if (sender == table && message.Is(JTableData::kRowsRemoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::RowsRemoved*>(&message);
		assert( info != nullptr );
		const JPoint origBoat   = itsBoat;
		const JPoint origAnchor = itsAnchor;
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		AdjustIndexAfterRemove(origBoat.y, origAnchor.y,
							   info->GetFirstIndex(), GetRowCount(),
							   &(itsBoat.y), &(itsAnchor.y));
		reselect = true;
	}

	// columns changed

	else if (sender == table && message.Is(JTableData::kColsInserted))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColsInserted*>(&message);
		assert( info != nullptr );
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		reselect = true;
	}

	else if (sender == table && message.Is(JTableData::kColDuplicated))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColDuplicated*>(&message);
		assert( info != nullptr );
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		reselect = true;
	}

	else if (sender == table && message.Is(JTableData::kColsRemoved))
	{
		const auto* info =
			dynamic_cast<const JTableData::ColsRemoved*>(&message);
		assert( info != nullptr );
		const JPoint origBoat   = itsBoat;
		const JPoint origAnchor = itsAnchor;
		info->AdjustCell(&itsBoat);
		info->AdjustCell(&itsAnchor);
		AdjustIndexAfterRemove(origBoat.x, origAnchor.x,
							   info->GetFirstIndex(), GetColCount(),
							   &(itsBoat.x), &(itsAnchor.x));
		reselect = true;
	}

	// elements changed

	else if (sender == table && message.Is(JTableData::kRectChanged))
	{
		reselect = true;
	}

	// everything changed

	else if (sender == table && message.Is(JTable::kTableDataChanged))
	{
		ClearSelection();
	}

	// select new area

	if (reselect && itsReselectAfterChangeFlag && OKToExtendSelection())
	{
		SelectRect(itsBoat, itsAnchor, true);
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

bool
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
		return true;
	}
	else
	{
		cell->Set(0,0);
		return false;
	}
}

/******************************************************************************
 GetFirstSelectedCell

	Return the first selected cell, starting from the upper left and going in
	the specified direction.

 ******************************************************************************/

bool
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

bool
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

bool
JTableSelection::InvertSelection
	(
	const bool& b
	)
{
	return !b;
}
