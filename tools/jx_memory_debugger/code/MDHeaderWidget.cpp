/******************************************************************************
 MDHeaderWidget.cpp

	BASE CLASS = JXColHeaderWidget

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDHeaderWidget.h"
#include "MDRecordList.h"
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <JFontManager.h>
#include <JColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MDHeaderWidget::MDHeaderWidget
	(
	JXTable*			table,
	MDRecordList*		list,
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
	JXColHeaderWidget(table, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsList(list)
{
	SetColTitle(1, JGetString("StateColTitle::MDHeaderWidget"));
	SetColTitle(2, JGetString("FileColTitle::MDHeaderWidget"));
	SetColTitle(3, JGetString("LineColTitle::MDHeaderWidget"));
	SetColTitle(4, JGetString("SizeColTitle::MDHeaderWidget"));
	SetColTitle(5, JString::empty);
	SetColTitle(6, JGetString("DateColTitle::MDHeaderWidget"));

	TurnOnColResizing();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

MDHeaderWidget::~MDHeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
MDHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JXDrawUpFrame(p, rect, kJXDefaultBorderWidth);

	JString str;
	GetColTitle(cell.x, &str);

	JSize underLines = 0;
	if (itsList->GetSortColumn() == cell.x)
		{
		underLines = 1;
		}

	const JFont font = JFontManager::GetFont(
		JFontManager::GetDefaultFontName(), JFontManager::GetDefaultRowColHeaderFontSize(),
		JFontStyle(kJTrue, kJFalse, underLines, kJFalse, JColorManager::GetBlackColor()));
	p.SetFont(font);
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
MDHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXColHeaderWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);

	JPoint cell;
	if (GetDragType() == kInvalidDrag && GetCell(pt, &cell))
		{
		itsList->SetSortColumn(cell.x);
		TableRefresh();
		}
}
