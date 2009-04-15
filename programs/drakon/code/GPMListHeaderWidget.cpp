/******************************************************************************
 GPMListHeaderWidget.cc

	BASE CLASS = JXColHeaderWidget

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GPMListHeaderWidget.h>
#include <GPMProcessList.h>

#include <gpmGlobals.h>

#include <JColormap.h>
#include <JPainter.h>

#include <jXConstants.h>
#include <jXPainterUtil.h>

#include <jAssert.h>

const JSize kFontSize = 10;

/******************************************************************************
 Constructor

 ******************************************************************************/

GPMListHeaderWidget::GPMListHeaderWidget
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
	SetColTitle(2, "PID");
	SetColTitle(3, "User");
	SetColTitle(4, "Nice");
	SetColTitle(5, "Size");
//	SetColTitle(6, "Resident");
//	SetColTitle(7, "Share");
	SetColTitle(6, "%CPU");
	SetColTitle(7, "%Memory");
	SetColTitle(8, "Time");
	SetColTitle(9, "Command");
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMListHeaderWidget::~GPMListHeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GPMListHeaderWidget::TableDrawCell
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
		str = JString(cell.x, 0);
		}

	JSize underLines = 0;
	if (itsList->ListColIsSelected(cell.x))
		{
		underLines = 1;
		}

	p.SetFont(JGetDefaultFontName(), kFontSize,
			  JFontStyle(kJTrue, kJFalse, underLines, kJFalse, (p.GetColormap())->GetBlackColor()));
	p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GPMListHeaderWidget::HandleMouseDown
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
		itsList->ListColSelected(cell.x);
		TableRefresh();
		}
}
