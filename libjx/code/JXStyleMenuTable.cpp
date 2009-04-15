/******************************************************************************
 JXStyleMenuTable.cpp

	BASE CLASS = JXTextMenuTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStyleMenuTable.h>
#include <JXStyleMenu.h>
#include <JPainter.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStyleMenuTable::JXStyleMenuTable
	(
	JXStyleMenu*		menu,
	JXTextMenuData*		data,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenuTable(menu, data, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsStyleMenu = menu;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStyleMenuTable::~JXStyleMenuTable()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXStyleMenuTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	origRect
	)
{
	if (cell.x == kTextColumnIndex && cell.y >= JXStyleMenu::kFirstColorCmd)
		{
		JRect rect = AdjustRectForSeparator(cell.y, origRect);

		JRect colorRect = rect;
		colorRect.Shrink(0, kHilightBorderWidth);
		colorRect.right = colorRect.left + colorRect.height();

		const JBoolean origFill = p.IsFilling();
		p.SetFilling(kJTrue);
		p.SetPenColor(itsStyleMenu->IndexToColor(cell.y));
		p.Rect(colorRect);
		p.SetFilling(origFill);

		rect = origRect;
		rect.left += colorRect.width() + kHMarginWidth;
		JXTextMenuTable::TableDrawCell(p, cell, rect);
		}
	else
		{
		JXTextMenuTable::TableDrawCell(p, cell, origRect);
		}
}
