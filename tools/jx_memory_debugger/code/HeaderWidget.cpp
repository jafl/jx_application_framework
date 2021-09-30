/******************************************************************************
 HeaderWidget.cpp

	BASE CLASS = JXColHeaderWidget

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include "HeaderWidget.h"
#include "RecordList.h"
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jx/jXConstants.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

HeaderWidget::HeaderWidget
	(
	JXTable*			table,
	RecordList*		list,
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
	SetColTitle(1, JGetString("StateColTitle::HeaderWidget"));
	SetColTitle(2, JGetString("FileColTitle::HeaderWidget"));
	SetColTitle(3, JGetString("LineColTitle::HeaderWidget"));
	SetColTitle(4, JGetString("SizeColTitle::HeaderWidget"));
	SetColTitle(5, JString::empty);
	SetColTitle(6, JGetString("DateColTitle::HeaderWidget"));

	TurnOnColResizing();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

HeaderWidget::~HeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
HeaderWidget::TableDrawCell
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
		JFontStyle(true, false, underLines, false, JColorManager::GetBlackColor()));
	p.SetFont(font);
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
HeaderWidget::HandleMouseDown
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
