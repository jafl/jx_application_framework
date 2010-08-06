/******************************************************************************
 GMessageColHeader.cc

	Maintains a row to match the columns of a JTable.

	BASE CLASS = JXEditTable

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <GMessageColHeader.h>
#include "GMMailboxData.h"

#include <JXWindow.h>
#include <JXColormap.h>

#include <JFontStyle.h>
#include <JPainter.h>
#include <JString.h>

#include <jXPainterUtil.h>
#include <jXGlobals.h>

#include <jAssert.h>

const JCoordinate kMinColSize = 10;

enum
	{
	kAdjustCol	= 2,
	kFromCol	= 4,
	kSubCol,
	kDateCol,
	kSizeCol
	};

/******************************************************************************
 Constructor

 ******************************************************************************/

GMessageColHeader::GMessageColHeader
	(
	JXTable*			table,
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
	itsData(NULL)
{
	TurnOnColResizing(kMinColSize);
//	SetBackColor(GetColormap()->GetWhiteColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMessageColHeader::~GMessageColHeader()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

	We provide a default implementation, for convenience.

 ******************************************************************************/

static JPolygon kUpTriangle, kDownTriangle;

void
GMessageColHeader::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	if (kUpTriangle.IsEmpty())
		{
		kUpTriangle.AppendElement(JPoint(4, 4));
		kUpTriangle.AppendElement(JPoint(1, 10));
		kUpTriangle.AppendElement(JPoint(7, 10));

		kDownTriangle.AppendElement(JPoint(4, 10));
		kDownTriangle.AppendElement(JPoint(1, 4));
		kDownTriangle.AppendElement(JPoint(7, 4));
		}
		
	JXDrawUpFrame(p, rect, kJXDefaultBorderWidth);
	
	JString str;
	JBoolean hasTitle = GetColTitle(cell.x, &str);
	p.SetFont(JGetDefaultFontName(), kJDefaultFontSize,
			  JFontStyle());//kJTrue, kJFalse, 0, kJFalse, (p.GetColormap())->GetBlackColor()));
	if (hasTitle)
		{		
		p.String(rect, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}

	if (cell.x >= kFromCol &&
		itsData != NULL &&
		itsData->GetSortType() == cell.x - kAdjustCol)
		{
		p.SetPenColor(GetColormap()->GetGrayColor(40));
		p.SetFilling(kJTrue);
		JPoint polyPt(rect.right - 15, 0);
		if (itsData->SortIsAscending())
			{
			p.Polygon(polyPt, kDownTriangle);			
			}
		else
			{
			p.Polygon(polyPt, kUpTriangle);
			}
		p.SetFilling(kJFalse);
		}
		
	p.SetPenColor(GetColormap()->GetDefaultBackColor());
	if (cell.x < (JCoordinate)GetColCount())
		{
		p.Line(rect.right - 1, rect.top, rect.right - 1, rect.bottom);
		}
}

/******************************************************************************
 SetData

 ******************************************************************************/

void
GMessageColHeader::SetData
	(
	GMMailboxData* data
	)
{
	itsData	= data;
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
GMessageColHeader::HandleMouseDown
	(
	const JPoint& pt,
	const JXMouseButton button,
	const JSize clickCount,
	const JXButtonStates& buttonStates,
	const JXKeyModifiers& modifiers
	)
{
	JPoint cell;
	const JBoolean inDragRegion = InDragRegion(pt, &cell);

	const JBoolean inCell		= GetCell(pt, &cell);
	
	if (inCell && !inDragRegion && itsData != NULL)
		{
		if (cell.x >= kFromCol)
			{
			itsData->ToggleSortType((GMMailboxData::kSortType)(cell.x - kAdjustCol));
			TableRefresh();
			}
		
		}
	JXColHeaderWidget::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
}


