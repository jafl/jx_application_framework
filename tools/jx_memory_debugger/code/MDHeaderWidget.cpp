/******************************************************************************
 MDHeaderWidget.cc

	BASE CLASS = JXColHeaderWidget

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "MDHeaderWidget.h"
#include "MDRecordList.h"
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <JFontManager.h>
#include <JXColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

const JSize kFontSize = 9;

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
	SetColTitle(1, "!");
	SetColTitle(2, "File");
	SetColTitle(3, "Line");
	SetColTitle(4, "Size");
	SetColTitle(5, "");
	SetColTitle(6, "Data");

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

	const JFont font = GetFontManager()->GetFont(
		JGetDefaultFontName(), kFontSize,
		JFontStyle(kJTrue, kJFalse, underLines, kJFalse, (p.GetColormap())->GetBlackColor()));
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
