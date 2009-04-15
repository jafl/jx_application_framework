/******************************************************************************
 JXTextMenuTable.cpp

	Class to draw a text menu.

	BASE CLASS = JXMenuTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTextMenuTable.h>
#include <JXTextMenuData.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <JXImage.h>
#include <JXColormap.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextMenuTable::JXTextMenuTable
	(
	JXMenu*				menu,
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
	JXMenuTable(menu, data, enclosure, hSizing,vSizing, x,y, w,h),
	itsTextMenuData(data),
	itsHilightRow(0)
{
	AppendCols(4);	// checkbox/radio, image, text, submenu/shortcut
	AppendRows(itsTextMenuData->GetElementCount());

	const_cast<JXTextMenuData*>(itsTextMenuData)->ConfigureTable(this);
	ListenTo(itsTextMenuData);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextMenuTable::~JXTextMenuTable()
{
}

/******************************************************************************
 Draw (virtual protected)

	We have to draw each separator and the selection box
	across the entire width of the table.

 ******************************************************************************/

void
JXTextMenuTable::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXMenuTable::Draw(p, rect);

	const JCoordinate w  = GetApertureWidth();
	const JSize rowCount = GetRowCount();
	for (JIndex i=1; i<rowCount; i++)	// ignore separator after last item
		{
		if (itsTextMenuData->HasSeparator(i))
			{
			JRect r = GetCellRect(JPoint(1,i));
			r.top   = r.bottom - kSeparatorHeight;
			r.right = r.left + w;
//			JXDrawDownFrame(p, r, kSeparatorHeight/2);

			r.top = r.ycenter() - 1;
			r.bottom = r.top + 2;
			JXDrawDownFrame(p, r, 1);
			}
		}

	if (itsHilightRow != 0)
		{
		const JRect r = GetCellRect(JPoint(1, itsHilightRow));
		JRect r1      = AdjustRectForSeparator(itsHilightRow, r);
		r1.right = r1.left + w;
		JXDrawUpFrame(p, r1, kHilightBorderWidth);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JXTextMenuTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	origRect
	)
{
	JRect rect = AdjustRectForSeparator(cell.y, origRect);

	if (cell.x == kCheckboxColumnIndex)
		{
		rect.left += kHilightBorderWidth;
		DrawCheckbox(p, cell.y, rect);
		}

	else if (cell.x == kImageColumnIndex)
		{
		const JXImage* image;
		if (itsTextMenuData->GetImage(cell.y, &image))
			{
			p.Image(*image, image->GetBounds(), rect);
			}
		}

	else if (cell.x == kTextColumnIndex)
		{
		JIndex ulIndex;
		JFontID id;
		JSize size;
		JFontStyle style;
		const JString& text =
			itsTextMenuData->GetText(cell.y, &ulIndex, &id, &size, &style);

		if (!itsTextMenuData->IsEnabled(cell.y))
			{
			style.color = (GetColormap())->GetInactiveLabelColor();
			}
		p.SetFont(id, size, style);

		rect.left += kHMarginWidth;

		JXWindowPainter* xp = dynamic_cast(JXWindowPainter*, &p);
		assert( xp != NULL );
		xp->String(rect.left, rect.top, text, ulIndex,
				   rect.width(), JPainter::kHAlignLeft,
				   rect.height(), JPainter::kVAlignCenter);
		}

	else if (cell.x == kSubmenuColumnIndex && itsTextMenuData->HasSubmenu(cell.y))
		{
		rect.right -= kHilightBorderWidth;
		rect.left   = rect.right - JXMenuTable::kSubmenuColWidth;
		DrawSubmenuIndicator(p, cell.y, rect,
							 JConvertToBoolean(((JIndex) cell.y) == itsHilightRow));
		}

	else if (cell.x == kSubmenuColumnIndex)
		{
		const JString* nmShortcut;
		JFontID id;
		JSize size;
		JFontStyle style;
		if (itsTextMenuData->GetNMShortcut(cell.y, &nmShortcut, &id, &size, &style))
			{
			if (!itsTextMenuData->IsEnabled(cell.y))
				{
				style.color = (GetColormap())->GetInactiveLabelColor();
				}
			p.SetFont(id, size, style);

			rect.left  += kHNMSMarginWidth;
			rect.right -= kHilightBorderWidth;
			p.String(rect, *nmShortcut, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
			}
		}
}

/******************************************************************************
 CellToItemIndex (virtual protected)

 ******************************************************************************/

JBoolean
JXTextMenuTable::CellToItemIndex
	(
	const JPoint&	pt,
	const JPoint&	cell,
	JIndex*			itemIndex
	)
	const
{
	if (JIndex(cell.y) < GetRowCount() && itsTextMenuData->HasSeparator(cell.y))
		{
		const JRect r = GetCellRect(cell);
		if (r.bottom - kSeparatorHeight <= pt.y && pt.y <= r.bottom)
			{
			*itemIndex = 0;
			return kJFalse;
			}
		}

	*itemIndex = cell.y;
	return kJTrue;
}

/******************************************************************************
 MenuHilightItem (virtual protected)

 ******************************************************************************/

void
JXTextMenuTable::MenuHilightItem
	(
	const JIndex itemIndex
	)
{
	itsHilightRow = itemIndex;
	TableRefreshRow(itemIndex);
}

/******************************************************************************
 MenuUnhilightItem (virtual protected)

 ******************************************************************************/

void
JXTextMenuTable::MenuUnhilightItem
	(
	const JIndex itemIndex
	)
{
	assert( itemIndex == itsHilightRow );
	itsHilightRow = 0;
	TableRefreshRow(itemIndex);
}

/******************************************************************************
 GetSubmenuPoints (virtual protected)

 ******************************************************************************/

void
JXTextMenuTable::GetSubmenuPoints
	(
	const JIndex	itemIndex,
	JPoint*			leftPt,
	JPoint*			rightPt
	)
{
	const JRect cellRect = GetCellRect(JPoint(1,itemIndex));
	const JRect ap       = GetAperture();
	*leftPt  = JPoint(ap.left+1,  cellRect.top);
	*rightPt = JPoint(ap.right-1, cellRect.top);
}

/******************************************************************************
 AdjustRectForSeparator (protected)

 ******************************************************************************/

JRect
JXTextMenuTable::AdjustRectForSeparator
	(
	const JIndex	rowIndex,
	const JRect&	rect
	)
{
	if (rowIndex < GetRowCount() && itsTextMenuData->HasSeparator(rowIndex))
		{
		JRect r   = rect;
		r.bottom -= kSeparatorHeight;
		return r;
		}
	else
		{
		return rect;
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXTextMenuTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == const_cast<JXTextMenuData*>(itsTextMenuData) &&
		message.Is(JXTextMenuData::kImageChanged))
		{
		const JXTextMenuData::ImageChanged* item =
			dynamic_cast(const JXTextMenuData::ImageChanged*, &message);
		assert( item != NULL );
		TableRefreshCell(JPoint(kImageColumnIndex, item->GetIndex()));
		}

	else
		{
		JXMenuTable::Receive(sender, message);
		}
}
