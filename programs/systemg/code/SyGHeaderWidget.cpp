/******************************************************************************
 SyGHeaderWidget.cpp

	Maintains a row to match the columns of a JTable.

	BASE CLASS = JXTable

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "SyGHeaderWidget.h"
#include "SyGFileTreeTable.h"
#include <JXWindow.h>
#include <JFontManager.h>
#include <JXColorManager.h>
#include <jXPainterUtil.h>
#include <jXGlobals.h>
#include <jAssert.h>

const JCoordinate kBorderWidth = 3;

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
	JColorManager* mgr = GetDisplay()->GetColorManager();

	SetRowBorderInfo(0, mgr->GetBlackColor());
	SetColBorderInfo(0, mgr->GetBlackColor());

	WantInput(false,false);	// we don't want focus
	SetBackColor(mgr->GetWhiteColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGHeaderWidget::~SyGHeaderWidget()
{
}

/******************************************************************************
 GetPreferredHeight (static)

 ******************************************************************************/

JCoordinate
SyGHeaderWidget::GetPreferredHeight
	(
	JFontManager* fontMgr
	)
{
	return JFontManager::GetDefaultFont().GetLineHeight(fontMgr) + 2*kBorderWidth;
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
SyGHeaderWidget::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JFont font = JFontManager::GetDefaultFont();
	if (itsTable->IsCurrentCol(cell.x))
		{
		font.SetUnderlineCount(1);
		}
	p.SetFont(font);

	const JString str				= itsTable->GetColTitle(cell.x);
	const JCoordinate bufferWidth	= itsTable->GetBufferWidth(cell.x);

	JRect sRect = rect;
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
