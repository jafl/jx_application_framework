/*******************************************************************************
 GloveCursorMonitor.cpp

	GloveCursorMonitor class.
 
	Copyright @ 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/
 
#include <glStdInc.h>
#include "GloveCursorMonitor.h"
#include "GlovePlotter.h"
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <JXWindow.h>
#include <jAssert.h>

const JSize kXBorderOffset = 10;
const JSize kYBorderOffset = 5;
const JSize kXValueSpacing = 5;
const int  kValuePrecision = 3;

/*******************************************************************************
 Constructor
 

 ******************************************************************************/

GloveCursorMonitor::GloveCursorMonitor
	(
	GlovePlotter* 		plot,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetBorderWidth(kJXDefaultBorderWidth);
	SetBackColor(GetColormap()->GetWhiteColor());
	itsPlot  = plot;
	itsShowX = kJFalse;
	itsShowY = kJFalse;
	itsDual  = kJFalse;
	ListenTo(itsPlot);
}

/*******************************************************************************
 Destructor
 

 ******************************************************************************/

GloveCursorMonitor::~GloveCursorMonitor()
{
}

/*******************************************************************************
 Draw
 

 ******************************************************************************/

void
GloveCursorMonitor::Draw
	(
	JXWindowPainter& p, 
	const JRect& rect
	)
{
	const JSize w = GetApertureWidth() - 2 * kXBorderOffset;
	JCoordinate secondPt = w/3 + kXBorderOffset;
	JCoordinate thirdPt  = 2 * w/3 + kXBorderOffset;
	
	if (itsShowY)
		{
		JSize current = kXBorderOffset;
		JString y1("y1 = ");
		y1 += JString(itsY1);
		p.String(current, kYBorderOffset, y1);
		if (itsDual)
			{
			current += p.GetStringWidth(y1) + kXValueSpacing;
			JString y2("y2 = ");
			y2 += JString(itsY2);
			p.String(secondPt, kYBorderOffset, y2);
			
			current += p.GetStringWidth(y2) + kXValueSpacing;
			JString dy("dy = ");
			dy += JString(itsY2 - itsY1);
			p.String(thirdPt, kYBorderOffset, dy);
			}
		}
	if (itsShowX)
		{
		JSize current = kXBorderOffset;
		const JSize lineheight = p.GetLineHeight();
		JString x1("x1 = ");
		x1 += JString(itsX1);
		p.String(current, kYBorderOffset + lineheight, x1);
		if (itsDual)
			{
			current += p.GetStringWidth(x1) + kXValueSpacing;
			JString x2("x2 = ");
			x2 += JString(itsX2);
			p.String(secondPt, kYBorderOffset + lineheight, x2);
			
			current += p.GetStringWidth(x2) + kXValueSpacing;
			JString dx("dx = ");
			dx += JString(itsX2 - itsX1);
			p.String(thirdPt, kYBorderOffset + lineheight, dx);
			}
		}
}

/*******************************************************************************
 DrawBorder
 

 ******************************************************************************/

void
GloveCursorMonitor::DrawBorder
	(
	JXWindowPainter& p, 
	const JRect& frame
	)
{
	JXDrawDownFrame(p, frame, GetBorderWidth());
}

/*******************************************************************************
 Receive
 

 ******************************************************************************/

void
GloveCursorMonitor::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsPlot && message.Is(GlovePlotter::kCursorsChanged))
		{
		 const GlovePlotter::CursorsChanged* info =
			dynamic_cast<const GlovePlotter::CursorsChanged*>(&message);
		assert( info != NULL );
		itsShowX = info->ShowX();
		itsShowY = info->ShowY();
		itsDual  = info->Dual();
		itsX1    = info->GetX1();
		itsX2    = info->GetX2();
		itsY1    = info->GetY1();
		itsY2    = info->GetY2();
		Refresh();
		}
				
	else
		{
		JXWidget::Receive(sender, message);
		}
}
