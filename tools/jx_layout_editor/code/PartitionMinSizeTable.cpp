/******************************************************************************
 PartitionMinSizeTable.cpp

	Draws a table of numbers stored in a JIntegerTableData object
	and buffered in a JIntegerBufferTableData object.

	BASE CLASS = JXIntegerTable

	Copyright (C) 2024 by John Lindal.

 ******************************************************************************/

#include "PartitionMinSizeTable.h"
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PartitionMinSizeTable::PartitionMinSizeTable
	(
	JIntegerTableData*	data,
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
	JXIntegerTable(data, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PartitionMinSizeTable::~PartitionMinSizeTable()
{
}

/******************************************************************************
 HandleMouseDown

 ******************************************************************************/

void
PartitionMinSizeTable::HandleMouseDown
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
PartitionMinSizeTable::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXIntegerTable::ApertureResized(dw,dh);
	SetColWidth(1, GetApertureWidth());
}

/******************************************************************************
 CreateIntegerTableInput (virtual protected)

 ******************************************************************************/

JXIntegerInput*
PartitionMinSizeTable::CreateIntegerTableInput
	(
	const JPoint&		cell,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
{
	auto* obj = jnew JXIntegerInput(enclosure, hSizing, vSizing, x,y, w,h);
	obj->SetLowerLimit(10);
	return obj;
}
