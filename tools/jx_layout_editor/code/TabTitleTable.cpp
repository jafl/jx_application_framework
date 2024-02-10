/******************************************************************************
 TabTitleTable.cpp

	Draws a table of numbers stored in a JIntegerTableData object
	and buffered in a JIntegerBufferTableData object.

	BASE CLASS = JXStringTable

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#include "TabTitleTable.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TabTitleTable::TabTitleTable
	(
	JStringTableData*	data,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStringTable(data, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TabTitleTable::~TabTitleTable()
{
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
TabTitleTable::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button > kJXRightButton)
	{
		ScrollForWheel(button, modifiers);
		return;
	}
	else if (!GetCell(pt, &cell))
	{
		return;
	}

	if (button == kJXLeftButton && clickCount == 1)
	{
		SelectSingleCell(cell);
	}
	else if (button == kJXLeftButton && clickCount == 2)
	{
		BeginEditing(cell);
	}
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
TabTitleTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXStringTable::ApertureResized(dw,dh);
	SetColWidth(1, GetApertureWidth());
}
