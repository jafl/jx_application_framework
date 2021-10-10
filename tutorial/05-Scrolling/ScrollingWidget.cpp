/******************************************************************************
 ScrollingWidget.cpp

	Widgets often need a larger drawing area than what is available
	in the window.  In order to do this, the widget must be able to
	scroll.  Such widgets must be derived from JXScrollableWidget
	and must be placed inside a JXScrollbarSet, as shown in
	ScrollingWidgetDir.

	We didn't need to mention it in the Widget example, but drawing
	is always done in the Bounds coordinate frame.  These coordinates
	are independent of scrolling, so Draw() can always do the same
	thing, no matter where the scrollbars are.

	JXScrollableWidget automatically provides a border, so we do
	not need to implement DrawBorder().

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "ScrollingWidget.h"
#include <JXWidget.h>
#include <JXWindowPainter.h>
#include <JXColorManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ScrollingWidget::ScrollingWidget
	(
	JXScrollbarSet* scrollbarSet,
	JXContainer* enclosure,
	const HSizingOption hSizing, 
	const VSizingOption vSizing,
	const JCoordinate x, 
	const JCoordinate y,
	const JCoordinate w, 
	const JCoordinate h
	)
	:
	JXScrollableWidget(scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
	// This changes our Bounds, independent of what part of us
	// is visible (our Frame).
	SetBounds(500, 400);
}

/******************************************************************************
 Draw

	This gets called by the event loop every time the Widget needs to
	be redrawn.

 ******************************************************************************/

void
ScrollingWidget::Draw
	(
	JXWindowPainter& p, 
	const JRect& rect
	)
{
	// Drawing goes here
	// See JPainter.h for available functions
	
	p.SetPenColor(JColorManager::GetGreenColor());
	p.Rect(10, 10, 50, 50);
	
	p.SetFilling(true);
	p.SetPenColor(JColorManager::GetBlueColor());
	p.Rect(10, 70, 50, 50);
}
