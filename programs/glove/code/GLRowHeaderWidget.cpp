/******************************************************************************
 GLRowHeaderWidget.cpp

	Maintains a column to match the rows of a JTable.

	BASE CLASS = JXEditTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "GLRowHeaderWidget.h"
#include "GLRaggedFloatTable.h"
#include <JXWindow.h>
#include <JXColorManager.h>
#include <JXScrollbarSet.h>
#include <JXScrollbar.h>
#include <JXDragPainter.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <JFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

const JSize kCellFrameWidth = kJXDefaultBorderWidth;

/******************************************************************************
 Constructor

 ******************************************************************************/

GLRowHeaderWidget::GLRowHeaderWidget
	(
	GLRaggedFloatTable*	table,
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
	JXRowHeaderWidget(table, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLRowHeaderWidget::~GLRowHeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

void
GLRowHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JXDrawUpFrame(p, rect, kCellFrameWidth);

	const JFont font = JFontManager::GetFont(
		JFontManager::GetDefaultFontName(), 10,
		JFontStyle(true, false, 0, false, JColorManager::GetBlackColor()));
	p.SetFont(font);
	JString str(cell.y, 0);
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GLRowHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint itsDragCell;
	const bool inDragRegion = InDragRegion(pt, &itsDragCell);

	if (inDragRegion && button == kJXLeftButton)
		{
		JXRowHeaderWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
		}

	else if (modifiers.shift() || button == kJXRightButton)
		{
		itsTable->ExtendSelectionToRow(itsDragCell.y);
		}
	
	else if (button == kJXLeftButton)
		{
		itsTable->SelectRow(itsDragCell.y);
		}

}

