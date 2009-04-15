/******************************************************************************
 JXImageMenuTable.cpp

	Class to draw a menu of icons.

	BASE CLASS = JXMenuTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImageMenuTable.h>
#include <JXImageMenuData.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <JXImage.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageMenuTable::JXImageMenuTable
	(
	JXMenu*				menu,
	JXImageMenuData*	data,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXMenuTable(menu, data, enclosure, hSizing,vSizing, x,y, w,h),
	itsIconMenuData( data ),
	itsHilightCell(0,0)
{
	const JPoint lastCell = ItemIndexToCell(itsIconMenuData->GetElementCount());
	AppendRows(lastCell.y);

	JSize colCount = itsIconMenuData->GetColumnCount();
	if (lastCell.y == 1)
		{
		colCount = lastCell.x;
		}
	AppendCols(colCount);

	data->ConfigureTable(this, &itsHasCheckboxesFlag, &itsHasSubmenusFlag);
	ListenTo(itsIconMenuData);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageMenuTable::~JXImageMenuTable()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXImageMenuTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	origRect
	)
{
	JIndex itemIndex;
	if (!CellToItemIndex(JPoint(0,0), cell, &itemIndex))
		{
		return;
		}

	const JBoolean hilight = JConvertToBoolean(cell == itsHilightCell);
	if (hilight)
		{
		JXDrawUpFrame(p, origRect, kHilightBorderWidth);
		}

	JRect rect = origRect;
	rect.Shrink(kHilightBorderWidth, kHilightBorderWidth);

	if (itsHasCheckboxesFlag)
		{
		JRect boxRect = rect;
		boxRect.right = boxRect.left + JXMenuTable::kCheckboxColWidth;
		DrawCheckbox(p, itemIndex, boxRect);
		rect.left = boxRect.right;
		}

	if (itsHasSubmenusFlag)
		{
		JRect arrowRect = rect;
		arrowRect.left  = arrowRect.right - JXMenuTable::kSubmenuColWidth;
		DrawSubmenuIndicator(p, itemIndex, arrowRect, hilight);
		rect.right = arrowRect.left;
		}

	const JXImage* image = itsIconMenuData->GetImage(itemIndex);
	p.Image(*image, image->GetBounds(), rect);
}

/******************************************************************************
 CellToItemIndex (virtual protected)

 ******************************************************************************/

JBoolean
JXImageMenuTable::CellToItemIndex
	(
	const JPoint&	pt,
	const JPoint&	cell,
	JIndex*			itemIndex
	)
	const
{
	*itemIndex = ((cell.y-1) * itsIconMenuData->GetColumnCount()) + cell.x;
	return itsIconMenuData->IndexValid(*itemIndex);
}

/******************************************************************************
 ItemIndexToCell (private)

 ******************************************************************************/

JPoint
JXImageMenuTable::ItemIndexToCell
	(
	const JIndex itemIndex
	)
	const
{
	const JSize colCount = itsIconMenuData->GetColumnCount();

	JCoordinate x = itemIndex % colCount;
	JCoordinate y = itemIndex / colCount;
	if (x != 0)
		{
		y++;
		}
	if (x == 0)
		{
		x = colCount;
		}

	return JPoint(x,y);
}

/******************************************************************************
 MenuHilightItem (virtual protected)

 ******************************************************************************/

void
JXImageMenuTable::MenuHilightItem
	(
	const JIndex itemIndex
	)
{
	itsHilightCell = ItemIndexToCell(itemIndex);
	TableRefreshCell(itsHilightCell);
}

/******************************************************************************
 MenuUnhilightItem (virtual protected)

 ******************************************************************************/

void
JXImageMenuTable::MenuUnhilightItem
	(
	const JIndex itemIndex
	)
{
	TableRefreshCell(itsHilightCell);
	itsHilightCell.x = 0;
	itsHilightCell.y = 0;
}

/******************************************************************************
 GetSubmenuPoints (virtual protected)

 ******************************************************************************/

void
JXImageMenuTable::GetSubmenuPoints
	(
	const JIndex	itemIndex,
	JPoint*			leftPt,
	JPoint*			rightPt
	)
{
	const JRect cellRect = GetCellRect(ItemIndexToCell(itemIndex));
	*leftPt  = JPoint(cellRect.left+1,  cellRect.top);
	*rightPt = JPoint(cellRect.right-1, cellRect.top);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXImageMenuTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == const_cast<JXImageMenuData*>(itsIconMenuData) &&
		message.Is(JXImageMenuData::kImageChanged))
		{
		const JXImageMenuData::ImageChanged* item =
			dynamic_cast(const JXImageMenuData::ImageChanged*, &message);
		assert( item != NULL );
		TableRefreshCell(ItemIndexToCell(item->GetIndex()));
		}

	else
		{
		JXMenuTable::Receive(sender, message);
		}
}
