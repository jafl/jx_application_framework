/******************************************************************************
 GPMTreeHeaderWidget.cpp

	BASE CLASS = JXColHeaderWidget

	Copyright (C) 2006 by John Lindal.

 *****************************************************************************/

#include "GPMTreeHeaderWidget.h"
#include "GPMProcessList.h"
#include "gpmGlobals.h"
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <JPainter.h>
#include <JFontManager.h>
#include <JXColorManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GPMTreeHeaderWidget::GPMTreeHeaderWidget
	(
	JXTable*			table,
	GPMProcessList*		list,
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
	SetColTitle(1, JString::empty);
	SetColTitle(2, JGetString("CommandColumnTitle::GPMListHeaderWidget"));
	SetColTitle(3, JString::empty);
	SetColTitle(4, JGetString("PIDColumnTitle::GPMListHeaderWidget"));
	SetColTitle(5, JGetString("UserColumnTitle::GPMListHeaderWidget"));
	SetColTitle(6, JGetString("NiceColumnTitle::GPMListHeaderWidget"));
	SetColTitle(7, JGetString("SizeColumnTitle::GPMListHeaderWidget"));
	SetColTitle(8, JGetString("CPUColumnTitle::GPMListHeaderWidget"));
	SetColTitle(9, JGetString("MemoryColumnTite::GPMListHeaderWidget"));
	SetColTitle(10, JGetString("TimeColumnTitle::GPMListHeaderWidget"));
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMTreeHeaderWidget::~GPMTreeHeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GPMTreeHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JXDrawUpFrame(p, rect, kJXDefaultBorderWidth);

	JString str;
	if (!GetColTitle(cell.x, &str))
		{
		str = JString((JUInt64) cell.x);
		}

	JSize underLines = 0;
	if (itsList->TreeColIsSelected(cell.x))
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
GPMTreeHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (GetCell(pt, &cell))
		{
		itsList->TreeColSelected(cell.x);
		TableRefresh();
		}
}
