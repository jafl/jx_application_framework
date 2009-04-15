/******************************************************************************
 SyGHeaderWidget.cpp

	Maintains a row to match the columns of a JTable.

	BASE CLASS = JXTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGHeaderWidget.h"
#include "SyGFileTreeTable.h"
#include <JXWindow.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGHeaderWidget::SyGHeaderWidget
	(
	SyGFileTreeTable*	table,
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
	itsTable(table)
{
	SetRowBorderInfo(0, (GetColormap())->GetBlackColor());
	SetColBorderInfo(0, (GetColormap())->GetBlackColor());

	WantInput(kJFalse,kJFalse);	// we don't want focus
	SetBackColor((GetColormap())->GetWhiteColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGHeaderWidget::~SyGHeaderWidget()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

void
SyGHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if ((cell.x == 1) && itsTable->HasFocus())
		{
		p.SetPenColor((GetColormap())->GetBlueColor());
		JCoordinate xc = rect.xcenter();
		JCoordinate yc = rect.ycenter();
		JRect eRect(yc - 2, xc - 2, yc + 2, xc + 2);
//		p.JPainter::Ellipse(eRect);
		}

	else
		{
		JSize ul = 0;
		if (itsTable->IsCurrentCol(cell.x))
			{
			ul = 1;
			}
		p.SetFont(JGetDefaultFontName(), kJDefaultFontSize,
			JFontStyle(kJFalse, kJFalse, ul, kJFalse, (GetColormap())->GetBlackColor()));
		JRect sRect = rect;

		JString str					= itsTable->GetColTitle(cell.x);
		const JCoordinate bufferWidth	= itsTable->GetBufferWidth(cell.x);

		if (bufferWidth < 0)
			{
			sRect.right += bufferWidth;
			p.String(sRect, str, JPainter::kHAlignRight, JPainter::kVAlignCenter);
			}

		else
			{
			sRect.left += bufferWidth;
			p.String(sRect, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
			}
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
SyGHeaderWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton	button,
	const JSize			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (GetCell(pt, &cell))
		{
		itsTable->SetCurrentColIndex(cell.x);
		TableRefresh();
		}
}
