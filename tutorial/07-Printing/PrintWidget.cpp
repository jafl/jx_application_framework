/******************************************************************************
 PrintWidget.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997. 

 ******************************************************************************/

#include "PrintWidget.h"
#include <jx-af/jx/JXWidget.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXDragPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JPagePrinter.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PrintWidget::PrintWidget
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

	// This array keeps track of the points that define the beginning and
	// ending of each line the is draw in the window.

	// See JCollection.h, JList.h, and JArray.h for functionality

	itsPoints = jnew JArray<JPoint>();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PrintWidget::~PrintWidget()
{
	// Unlike widgets, which are automatically deleted by the framework,
	// we must delete this JArray since it is a private instance variable.
	jdelete itsPoints;
}

/*****************************************************************************
 Print

 ******************************************************************************/

void
PrintWidget::Print
	(
	JPagePrinter& p
	)
{
	// Calculate the height needed for the header
	const JCoordinate headerHeight = p.GetLineHeight();

	// Start the print document
	p.OpenDocument();

	// This will return false if the print job was cancelled
	if (p.NewPage())
	{
		// draw the header

		JRect pageRect = p.GetPageRect();
		p.String(pageRect.left, pageRect.top, JGetString("Text::PrintWidget"),
				 pageRect.width(), JPainter::HAlign::kCenter);
		p.LockHeader(headerHeight);

		// draw the page
		// Printing and drawing to the screen use the same code
		DrawStuff(p);

		// Finish the print job
		p.CloseDocument();
	}
}

/******************************************************************************
 Draw (virtual protected)

	This gets called by the event loop every time the Widget needs to
	be redrawn.

 ******************************************************************************/

void
PrintWidget::Draw
	(
	JXWindowPainter& p, 
	const JRect& rect
	)
{
	// This needs to be out of the main drawing routing, because
	// we don't want this to print
	p.String(10, 10, JGetString("Instructions::PrintWidget"), 200,
			 JPainter::HAlign::kLeft, p.GetLineHeight());

	// Call the generic drawing routing (same for printing)
	DrawStuff(p);
}

/******************************************************************************
 DrawStuff (private)

 ******************************************************************************/

void
PrintWidget::DrawStuff
	(
	JPainter& p
	)
{
	// Set pen color
	p.SetPenColor(JColorManager::GetBlackColor());

	// Find out how many points there are
	// There are count/2 lines
	JSize count = itsPoints->GetItemCount();

	// Loop through the points by twos
	for (JSize i = 1; i <= count; i += 2)
	{
		// Draw the line
		p.Line(itsPoints->GetItem(i), itsPoints->GetItem(i+1));
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

	This gets called by the event loop every time a mouse button is pressed.

	pt is where the mouse was clicked, in Bounds coordinates.

	button is the index of the mouse button being pressed.

	clickCount is the number of times that this button has been pressed
	in rapid succession.

	buttonStates tells the state of all the mouse buttons.
	Refer to JXButtonStates.h for more information.

	modifiers contains information about modifier keys like Shift and
	Control.  Refer to JXKeyModifiers.h for more information.

 ******************************************************************************/

void
PrintWidget::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	// Check to see if the left button was pressed
	if (button == kJXLeftButton)
	{
		// Create the drag painter to draw the rubber-band like lines
		JPainter* p = CreateDragInsidePainter();

		// Start the first line
		p->Line(pt, pt);
	}

	// Let the base class handle the wheel mouse.
	else
	{
		ScrollForWheel(button, modifiers);
	}

	// Initialize the current points
	itsStartPt = itsPrevPt = pt;
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

	This gets called as often as possible by the event loop as long as
	a mouse button is pressed.

	pt is where the mouse is, in Bounds coordinates.

	buttonStates tells the state of all the mouse buttons.
	Refer to JXButtonStates.h for more information.

	modifiers contains information about modifier keys like Shift and
	Control.  Refer to JXKeyModifiers.h for more information.

 ******************************************************************************/

void
PrintWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	// Check to see if the window was scrolled
	const bool scrolled = ScrollForDrag(pt);

	// Get the drag painter that we created in mouse down
	JPainter* p = nullptr;
	if (!GetDragPainter(&p))
	{
		return;
	}

	// Make sure that the left button is pressed, 
	// that we have moved,
	// and that a drag painter exists
	if (buttonStates.left() && pt != itsPrevPt && p != nullptr)	// p is nullptr for multiple click
	{

		// Draw line depending on whether or not we scrolled
		if (!scrolled)
		{
			p->Line(itsStartPt, itsPrevPt);
		}
		p->Line(itsStartPt, pt);
	}

	// Remember the current point
	itsPrevPt = pt;
}

/******************************************************************************
 HandleMouseUp (virtual protected)

	This gets called by the event loop every time a mouse button is released.

	pt is where the mouse was released, in Bounds coordinates.

	button is the index of the mouse button being pressed.

	buttonStates tells the state of all the mouse buttons.
	Refer to JXButtonStates.h for more information.

	modifiers contains information about modifier keys like Shift and
	Control.  Refer to JXKeyModifiers.h for more information.

 ******************************************************************************/

void
PrintWidget::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPainter* p = nullptr;

	// Make sure that the left button is pressed, 
	// and that a drag painter exists
	if (button == kJXLeftButton && GetDragPainter(&p))
	{
		// Erase the last line that was drawn
		p->Line(itsStartPt, itsPrevPt);

		// Delete the drag painter
		DeleteDragPainter();

		// Add this set of points to our JArray
		itsPoints->AppendItem(itsStartPt);
		itsPoints->AppendItem(itsPrevPt);

		// Tell the widget to redraw itself
		Refresh();
	}
}

/******************************************************************************
 HandleKeyPress (virtual)

	This gets called by the event loop every time a key is pressed.

	If the key that was pressed had an ASCII equivalent, then 'key'
	contains the ASCII value.  Otherwise, 'key' contains the X KeySym
	value, as defined in X11/keysym.h.

	modifiers contains information about modifier keys like Shift and
	Control.  Refer to JXKeyModifiers.h for more information.

 ******************************************************************************/

void
PrintWidget::HandleKeyPress
	(
	const JUtf8Character& c,
	const int             keySym,				
	const JXKeyModifiers& modifiers
	)
{
	// Check if the 'c' key was pressed
	// If so, we want to clear the window
	if (c == 'c')
	{
		// remove all of the points from the JArray
		itsPoints->RemoveAll();

		// Redraw 
		Refresh();
	}
		
	// Check if the 'q' key was pressed
	else if (c == 'q')
	{
		// Quit the application if 'q' was pressed
		JXGetApplication()->Quit();
	}
		
	// If anything else was pressed, pass it up the inheritance tree
	else
	{
		JXScrollableWidget::HandleKeyPress(c, keySym, modifiers);
	}
}
