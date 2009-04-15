/******************************************************************************
 Widget.cpp

	Each area inside a window that displays a particular piece of
	information is an object derived from JXWidget.  To display
	something that is specialized for your particular application,
	you must create a new class derived from JXWidget.

	The minimum requirement for classes derived from JXWidget is that
	they must implement the Draw() and DrawBorder() functions, as
	shown here.

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "Widget.h"
#include <JXWidget.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

Widget::Widget
	(
	JXContainer* enclosure,
	const HSizingOption hSizing, 
	const VSizingOption vSizing,
	const JCoordinate x, 
	const JCoordinate y,
	const JCoordinate w, 
	const JCoordinate h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x, y, w, h)
{
	// We have to tell JXWidget that we want a border.
	// Only then will DrawBorder() get called.
	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

Widget::~Widget()
{
}

/******************************************************************************
 Draw

	This gets called by the event loop every time the Widget needs to
	be redrawn.

	p is the Painter that you use to draw to the screen.  The main reason
	for Painter is to hide the system dependent details of drawing,
	and to provide a uniform interface for drawing to the screen, to an
	offscreen image, and to a printer.

	rect is the area that needs to be redrawn.  In simple cases, you
	can just draw everything.  For complex widgets, you often want to
	optimize and only draw the part inside rect.

 ******************************************************************************/

void
Widget::Draw
	(
	JXWindowPainter& p, 
	const JRect&     rect
	)
{
	JXColormap* cmap = GetColormap();

	// This is where everything happens
	// See JPainter.h for available functions

	p.SetPenColor(cmap->GetGreenColor());
	p.Rect(10, 10, 50, 50);
	
	p.SetFilling(kJTrue);
	p.SetPenColor(cmap->GetBlueColor());
	p.Rect(10, 70, 50, 50);
}

/******************************************************************************
 DrawBorder

	This gets called by the event loop every time the Widget's border
	needs to be redrawn.

 ******************************************************************************/

void
Widget::DrawBorder
	(
	JXWindowPainter& p, 
	const JRect&     frame
	)
{
	JXDrawDownFrame(p, frame, kJXDefaultBorderWidth);
}
