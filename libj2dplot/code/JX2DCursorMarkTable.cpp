/******************************************************************************
 JX2DCursorMarkTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JX2DCursorMarkTable.h"

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jx/JXWindow.h>

#include "J2DPlotWidget.h"

#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>

#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/JMinMax.h>

#include <jx-af/jcore/jAssert.h>

const JCoordinate kDefColWidth  = 120;
const JCoordinate kHMarginWidth = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DCursorMarkTable::JX2DCursorMarkTable
	(
	J2DPlotWidget*		plot,
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
	JXTable(1,kDefColWidth, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h)
{
	itsPlot         = plot;
	itsPrintingFlag = false;

	AppendCols(2, kDefColWidth);
	SetColBorderInfo(1, JColorManager::GetGrayColor(80));
	SetRowBorderInfo(1, JColorManager::GetGrayColor(80));

	const JSize lineHeight = JFontManager::GetDefaultFont().GetLineHeight(GetFontManager());
	SetDefaultRowHeight(lineHeight + 2);
	AppendRows(1);

	ListenTo(plot);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DCursorMarkTable::~JX2DCursorMarkTable()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
JX2DCursorMarkTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	JRect r = rect;
	r.left  += kHMarginWidth;
	r.right -= kHMarginWidth;

	if (cell.y == 1 && cell.x == 1)
	{
		p.String(r, JGetString("XColumnTitle::JX2DCursorMarkTable"),
			itsPrintingFlag ? JPainter::HAlign::kLeft : JPainter::HAlign::kCenter,
			JPainter::VAlign::kCenter);
	}
	else if (cell.y == 1 && cell.x == 2)
	{
		p.String(r, JGetString("YColumnTitle::JX2DCursorMarkTable"),
			itsPrintingFlag ? JPainter::HAlign::kLeft : JPainter::HAlign::kCenter,
			JPainter::VAlign::kCenter);
	}
	else if (cell.x == 1 && itsPlot->GetXMarkCount() >= JSize(cell.y - 1))
	{
		const JFloat value = itsPlot->GetXMarkValue(cell.y - 1);
		const JString str  = J2DPlotWidget::FloatToString(value);
		p.String(r, str, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
	}
	else if (cell.x == 2 && itsPlot->GetYMarkCount() >= JSize(cell.y - 1))
	{
		const JFloat value = itsPlot->GetYMarkValue(cell.y - 1);
		const JString str  = J2DPlotWidget::FloatToString(value);
		p.String(r, str, JPainter::HAlign::kLeft, JPainter::VAlign::kCenter);
	}
}

/*******************************************************************************
 Receive

 ******************************************************************************/

void
JX2DCursorMarkTable::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPlot && message.Is(J2DPlotWidget::kCursorMarked))
	{
		JXWindowDirector* dir = GetWindow()->GetDirector();
		if (!dir->IsActive())
		{
			dir->Activate();
		}
		AdjustTable();
	}
	else if (sender == itsPlot && message.Is(J2DPlotWidget::kMarkRemoved))
	{
		JXWindowDirector* dir = GetWindow()->GetDirector();
		if (!dir->IsActive())
		{
			dir->Activate();
		}
		AdjustTable();
	}
	else
	{
		JXTable::Receive(sender, message);
	}
}

/*******************************************************************************
 AdjustTable

 ******************************************************************************/

void
JX2DCursorMarkTable::AdjustTable()
{
	const JSize xCount = itsPlot->GetXMarkCount();
	const JSize yCount = itsPlot->GetYMarkCount();
	if (yCount > 0 || xCount > 0)
	{
		const JSize count = JMax(xCount, yCount) + 1;
		if (GetRowCount() < count)
		{
			AppendRows(count - GetRowCount());
		}
		else if (GetRowCount() > count)
		{
			RemoveNextRows(count+1, GetRowCount() - count);
		}
	}
	else if (GetRowCount() > 1)
	{
		RemoveNextRows(2, GetRowCount()-1);
	}

	// in case nothing had to be added or removed

	Refresh();
}

/******************************************************************************
 Print

 ******************************************************************************/

bool
JX2DCursorMarkTable::Print
	(
	JPagePrinter&	p,
	const bool	putOnSamePage,
	const JRect&	partialPageRect
	)
{
	const bool hasXMarks = itsPlot->GetXMarkCount() > 0;
	const bool hasYMarks = itsPlot->GetYMarkCount() > 0;
	if (!hasXMarks && !hasYMarks)
	{
		return true;
	}

	JCoordinate lineWidth;
	JColorID color;
	GetColBorderInfo(&lineWidth, &color);
	SetColBorderInfo(0, color);
	SetRowBorderInfo(0, color);

	JIndex currentRow = 1;
	JCoordinate width = 0;
	if (hasXMarks)
	{
		width += GetColWidth(1);
	}
	if (hasYMarks)
	{
		width += GetColWidth(1);
	}

	// This fills the end of the first page

	itsPrintingFlag = true;

	bool keepGoing = true, result = true;
	if (putOnSamePage)
	{
		assert( !partialPageRect.IsEmpty() );
		keepGoing = DrawRegions(p, partialPageRect, width, &currentRow);
	}

	// Now we start filling other pages

	while (keepGoing)
	{
		if (!p.NewPage())
		{
			result = false;
			break;
		}
		keepGoing = DrawRegions(p, p.GetPageRect(), width, &currentRow);
	}

	itsPrintingFlag = false;

	SetColBorderInfo(lineWidth, color);
	SetRowBorderInfo(lineWidth, color);
	return result;
}

/******************************************************************************
 DrawRegions (private)

	Returns false when the last row has been drawn.

 ******************************************************************************/

bool
JX2DCursorMarkTable::DrawRegions
	(
	JPainter&			p,
	const JRect&		pageRect,
	const JCoordinate	regionWidth,
	JIndex*			currentRow
	)
{
	const JCoordinate pageWidth = pageRect.width();
	const JSize regionsPerPage  = pageWidth/regionWidth;
	for (JIndex i=1; i<=regionsPerPage; i++)
	{
		JRect regionRect = pageRect;
		regionRect.left  = regionRect.left + (i - 1) * regionWidth;
		regionRect.right = regionRect.left + regionWidth;
		const JSize rows = regionRect.height()/GetRowHeight(1);

		JCoordinate bottomRow = *currentRow + rows - 1;
		JCoordinate top = (*currentRow - 1) * GetRowHeight(1);
		JRect cellRect(top, 0, rows * GetRowHeight(1) + top, regionWidth);
		*currentRow = bottomRow + 1;
		TableDrawCells(p, cellRect, regionRect);

		if (*currentRow > GetRowCount())
		{
			return false;
		}
	}
	return true;
}

/******************************************************************************
 TableDrawCells

 ******************************************************************************/

void
JX2DCursorMarkTable::TableDrawCells
	(
	JPainter&		p,
	const JRect&	cellRect,
	const JRect&	regionRect
	)
{
	const JPoint start = cellRect.topLeft() - regionRect.topLeft();
	p.SetClipRect(regionRect);
	p.ShiftOrigin(-start);
	TableDraw(p, cellRect);
	p.ShiftOrigin(start);
	p.Reset();
}
