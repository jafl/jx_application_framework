/******************************************************************************
 JX2DCursorMarkTable.cpp

	BASE CLASS = JXEditTable

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <JX2DCursorMarkTable.h>

#include <JXWindowDirector.h>
#include <JXWindow.h>

#include <J2DPlotWidget.h>

#include <JPagePrinter.h>
#include <JFontManager.h>
#include <JColorManager.h>

#include <jGlobals.h>
#include <JMinMax.h>

#include <jAssert.h>

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
	itsPrintingFlag = kJFalse;

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
			itsPrintingFlag ? JPainter::kHAlignLeft : JPainter::kHAlignCenter,
			JPainter::kVAlignCenter);
		}
	else if (cell.y == 1 && cell.x == 2)
		{
		p.String(r, JGetString("YColumnTitle::JX2DCursorMarkTable"),
			itsPrintingFlag ? JPainter::kHAlignLeft : JPainter::kHAlignCenter,
			JPainter::kVAlignCenter);
		}
	else if (cell.x == 1 && itsPlot->GetXMarkCount() >= JSize(cell.y - 1))
		{
		const JFloat value = itsPlot->GetXMarkValue(cell.y - 1);
		const JString str  = J2DPlotWidget::FloatToString(value);
		p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
		}
	else if (cell.x == 2 && itsPlot->GetYMarkCount() >= JSize(cell.y - 1))
		{
		const JFloat value = itsPlot->GetYMarkValue(cell.y - 1);
		const JString str  = J2DPlotWidget::FloatToString(value);
		p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
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

JBoolean
JX2DCursorMarkTable::Print
	(
	JPagePrinter&	p,
	const JBoolean	putOnSamePage,
	const JRect&	partialPageRect
	)
{
	const JBoolean hasXMarks = JConvertToBoolean(itsPlot->GetXMarkCount() > 0);
	const JBoolean hasYMarks = JConvertToBoolean(itsPlot->GetYMarkCount() > 0);
	if (!hasXMarks && !hasYMarks)
		{
		return kJTrue;
		}

	JCoordinate lineWidth;
	JColorID color;
	GetColBorderInfo(&lineWidth, &color);
	SetColBorderInfo(0, color);
	SetRowBorderInfo(0, color);

	JIndex currentRow = 1;
	JSize cols        = 0;
	JCoordinate width = 0;
	if (hasXMarks)
		{
		cols  = 1;
		width = GetColWidth(1);
		}
	if (hasYMarks)
		{
		cols  += 1;
		width += GetColWidth(1);
		}

	// This fills the end of the first page

	itsPrintingFlag = kJTrue;

	JBoolean keepGoing = kJTrue, result = kJTrue;
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
			result = kJFalse;
			break;
			}
		keepGoing = DrawRegions(p, p.GetPageRect(), width, &currentRow);
		}

	itsPrintingFlag = kJFalse;

	SetColBorderInfo(lineWidth, color);
	SetRowBorderInfo(lineWidth, color);
	return result;
}

/******************************************************************************
 DrawRegions (private)

	Returns kJFalse when the last row has been drawn.

 ******************************************************************************/

JBoolean
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
			return kJFalse;
			}
		}
	return kJTrue;
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
