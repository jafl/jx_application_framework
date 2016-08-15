/******************************************************************************
 GloveCursorTable.cpp

	BASE CLASS = JXEditTable

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GloveCursorTable.h>
#include <GloveHistoryDir.h>

#include <JXWindowDirector.h>
#include <JXWindow.h>
#include <JXColormap.h>

#include <J2DPlotWidget.h>

#include <JPainter.h>
#include <JFontManager.h>
#include <JFontStyle.h>
#include <JString.h>

#include <jGlobals.h>
#include <JMinMax.h>

#include <jAssert.h>

const JSize kDefaultSize = kJDefaultFontSize;

const JCoordinate kDefColWidth  = 120;
const JCoordinate kHMarginWidth = 5;
const JCoordinate kVMarginWidth = 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GloveCursorTable::GloveCursorTable
	(
	GloveHistoryDir* 	session,
	J2DPlotWidget* 		plot,
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
	JXTable(1,kDefColWidth, scrollbarSet, enclosure,
				hSizing,vSizing, x,y, w,h)
{
	itsPlot = plot;
	AppendCols(2, kDefColWidth);
	SetColBorderInfo(1, GetColormap()->GetGrayColor(80));
	SetRowBorderInfo(1, GetColormap()->GetGrayColor(80));
	
	const JFontManager* fm = GetFontManager();
	JSize lineHeight = fm->GetDefaultFont().GetLineHeight();
	AppendRows(1, lineHeight + 2);
	
	itsShowX = kJFalse;
	itsShowY = kJFalse;
	itsDual  = kJFalse;
	ListenTo(plot);
	itsSession = session;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GloveCursorTable::~GloveCursorTable()
{
}

/******************************************************************************
 TableDrawCell (virtual protected)


 ******************************************************************************/

void
GloveCursorTable::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	p.SetFont(GetFontManager()->GetDefaultFont());
	JRect r = rect;
	r.left += kHMarginWidth;
	r.right -= kHMarginWidth;
	if (cell.y == 1)
		{
		JString str;
		if (cell.x == 1)
			{
			str = "X Marks";
			}
		else
			{
			str = "Y Marks";
			}
		p.String(r, str, JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else 
		{
		if (cell.x == 1)
			{
			if ((JCoordinate)(itsPlot->GetXMarkCount()) >= cell.y - 1)
				{
				JFloat value = itsPlot->GetXMarkValue(cell.y - 1);
				JString str(value);
				p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
				}
			}
		else
			{
			if ((JCoordinate)(itsPlot->GetYMarkCount()) >= cell.y - 1)
				{
				JFloat value = itsPlot->GetYMarkValue(cell.y - 1);
				JString str(value);
				p.String(r, str, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
				}
			}
		}
}

/*******************************************************************************
 Receive
 

 ******************************************************************************/

void
GloveCursorTable::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
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
GloveCursorTable::AdjustTable()
{
	JSize xCount = itsPlot->GetXMarkCount();
	JSize yCount = itsPlot->GetYMarkCount();
	if (yCount > 0 || xCount > 0)
		{
		JSize count = JMax(xCount, yCount) + 1;
		if (GetRowCount() < count)
			{
			const JFontManager* fm = GetFontManager();
			JSize lineHeight = fm->GetDefaultFont().GetLineHeight();
			
			for (JSize i = GetRowCount(); i < count; i++)
				{
				AppendRows(1, lineHeight + 2);
				}
			}
		else if (GetRowCount() > count)
			{
			JSize rCount = GetRowCount();
			for (JSize i = rCount; i > count; i--)
				{
				RemoveRow(i);
				}
			}
		}
	else
		{
		JSize count = GetRowCount();
		for (JSize i = count; i > 1; i--)
			{
			RemoveRow(i);
			}
		}
	TableRefresh();
}

/******************************************************************************
 SendToSession

 ******************************************************************************/

void
GloveCursorTable::SendToSession()
{
	JString str;
	
	if (itsShowX)
		{
		str = "X Cursors";
		itsSession->AppendText(str);
		str = "x1 = " + JString(itsX1);
		itsSession->AppendText(str);
		if (itsDual)
			{
			str = "x1 = " + JString(itsX2);
			itsSession->AppendText(str);
			str = "dx = " + JString(itsX2 - itsX1);	
			itsSession->AppendText(str);
			}
		itsSession->AppendText("\n");
		}
	if (itsShowY)
		{
		str = "Y Cursors";
		itsSession->AppendText(str);
		str = "y1 = " + JString(itsY1);
		itsSession->AppendText(str);
		if (itsDual)
			{
			str = "y1 = " + JString(itsY2);
			itsSession->AppendText(str);
			str = "dy = " + JString(itsY2 - itsY1);	
			itsSession->AppendText(str);
			}
		itsSession->AppendText("\n");
		}
	JSize count = itsPlot->GetXMarkCount();
	if (count > 0)
		{
		str = "X Marks";
		itsSession->AppendText(str);
		for (JSize i = 1; i <= count; i++)
			{
			JFloat value = itsPlot->GetXMarkValue(i);
			str = JString(value);
			itsSession->AppendText(str);
			}
		itsSession->AppendText("\n");
		}
	count = itsPlot->GetYMarkCount();
	if (count > 0)
		{
		str = "Y Marks";
		itsSession->AppendText(str);
		for (JSize i = 1; i <= count; i++)
			{
			JFloat value = itsPlot->GetYMarkValue(i);
			str = JString(value);
			itsSession->AppendText(str);
			}
		itsSession->AppendText("\n");
		}
}
