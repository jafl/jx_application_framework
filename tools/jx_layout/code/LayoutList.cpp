/******************************************************************************
 LayoutList.cpp

	BASE CLASS = JXStringList

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "LayoutList.h"
#include "MainDocument.h"
#include <jx-af/jcore/JTableSelection.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LayoutList::LayoutList
	(
	MainDocument*		doc,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStringList(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	itsDoc(doc)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LayoutList::~LayoutList()
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
LayoutList::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JTableSelection& s = GetTableSelection();

	JPoint cell;
	if (button > kJXRightButton)
	{
		ScrollForWheel(button, modifiers);
	}
	else if (!GetCell(pt, &cell))
	{
		s.ClearSelection();
	}
	else if (button == kJXLeftButton && modifiers.shift())
	{
		s.InvertCell(cell);
	}
	else if (button == kJXLeftButton)
	{
		s.ClearSelection();
		s.SelectCell(cell);

		if (clickCount == 2)
		{
			OpenSelectedLayouts();
		}
	}
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
LayoutList::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == '\n')
	{
		OpenSelectedLayouts();
	}
	else
	{
		JXStringList::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 OpenSelectedLayouts (private)

 ******************************************************************************/

void
LayoutList::OpenSelectedLayouts()
	const
{
	const JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);

	const JPtrArray<JString>& names = GetStringList();

	JPoint cell;
	while (iter.Next(&cell))
	{
		itsDoc->OpenLayout(*names.GetItem(cell.y));
	}
}
