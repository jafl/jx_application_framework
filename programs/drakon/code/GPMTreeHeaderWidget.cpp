/******************************************************************************
 GPMTreeHeaderWidget.cc

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
#include <JColormap.h>
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
	SetColTitle(1, "");
	SetColTitle(2, "Command");
	SetColTitle(3, "");
	SetColTitle(4, "PID");
	SetColTitle(5, "User");
	SetColTitle(6, "Nice");
	SetColTitle(7, "Size");
//	SetColTitle(8, "Resident");
//	SetColTitle(9, "Share");
	SetColTitle(8, "%CPU");
	SetColTitle(9, "%Memory");
	SetColTitle(10, "Time");
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
		str = JString(cell.x, JString::kBase10);
		}

	JSize underLines = 0;
	if (itsList->TreeColIsSelected(cell.x))
		{
		underLines = 1;
		}

	const JFont font = GetFontManager()->GetFont(
		JGetDefaultFontName(), JGetDefaultRowColHeaderFontSize(),
		JFontStyle(kJTrue, kJFalse, underLines, kJFalse, (p.GetColormap())->GetBlackColor()));
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
