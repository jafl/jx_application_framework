/******************************************************************************
 GXColHeaderWidget.cpp

	Maintains a row to match the columns of a JTable.

	BASE CLASS = JXEditTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "GXColHeaderWidget.h"
#include "GXRaggedFloatTable.h"
#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXDragPainter.h>
#include <JXFontManager.h>
#include <JXColorManager.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <jGlobals.h>
#include <jAssert.h>

const JSize kCellFrameWidth            = kJXDefaultBorderWidth;
const JCoordinate kDragRegionHalfWidth = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

GXColHeaderWidget::GXColHeaderWidget
	(
	GXRaggedFloatTable*	table,
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
	JXColHeaderWidget(table, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXColHeaderWidget::~GXColHeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

void
GXColHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JXDrawUpFrame(p, rect, kCellFrameWidth);

	const JFont font = GetFontManager()->GetFont(
		JGetDefaultFontName(), 10,
		JFontStyle(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetBlackColor()));
	p.SetFont(font);
	JString str(cell.x, 0);
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GXColHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	
	if (!GetCell(pt,&cell))
		{
		return;
		}
		
	JPoint itsDragCell;

	const JBoolean inDragRegion = InDragRegion(pt, &itsDragCell);

	if (inDragRegion && button == kJXLeftButton)
		{
		JXColHeaderWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}
		
	else if (modifiers.shift() || button == kJXRightButton)
		{
		itsTable->ExtendSelectionToCol(itsDragCell.x);
		}
	
	else if (button == kJXLeftButton)
		{
		itsTable->SelectCol(itsDragCell.x);
		}
}

