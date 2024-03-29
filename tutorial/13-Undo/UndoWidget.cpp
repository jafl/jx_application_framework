/******************************************************************************
 UndoWidget.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "UndoWidget.h"
#include "UndoLine.h"
#include "RedoLine.h"
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXWidget.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/JXDragPainter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

UndoWidget::UndoWidget
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
	itsUndoChain = jnew JUndoRedoChain(true);

	// This changes our Bounds, independent of what part of us
	// is visible (our Frame).
	SetBounds(500, 400);

	// This array keeps track of the points that define the beginning and
	// ending of each line the is draw in the window.

	// See JCollection.h, JList.h, and JArray.h for functionality

	itsPoints = jnew JArray<JPoint>;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoWidget::~UndoWidget()
{
	// Unlike widgets, which are automatically deleted by the framework,
	// we must delete this JArray since it is a private instance variable.
	jdelete itsPoints;

	// This object must also be deleted.
	jdelete itsUndoChain;
}

/******************************************************************************
 Draw (virtual protected)

	This gets called by the event loop every time the Widget needs to
	be redrawn.

 ******************************************************************************/

void
UndoWidget::Draw
	(
	JXWindowPainter& p,
	const JRect&     rect
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
		// We need to specify that this is a JPainter function because
		// JXWindowPainter has this function in a different form
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
UndoWidget::HandleMouseDown
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
UndoWidget::HandleMouseDrag
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
UndoWidget::HandleMouseUp
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

		// Create the undo object to undo the addition of this line

		UndoLine* undo = jnew UndoLine(this);
		itsUndoChain->NewUndo(undo);

		// Tell the widget to redraw itself
		Refresh();
	}
}

/******************************************************************************
 Undo (public)

	This is the function that is called when the user asks to undo.

 ******************************************************************************/

void
UndoWidget::Undo()
{
	itsUndoChain->Undo();
}

/******************************************************************************
 Redo (public)

	This is the function that is called when the user asks to redo.

 ******************************************************************************/

void
UndoWidget::Redo()
{
	itsUndoChain->Redo();
}

/******************************************************************************
 AddLine (private)

	This is called by a RedoLine object.

 ******************************************************************************/

void
UndoWidget::AddLine
	(
	const JPoint& start,
	const JPoint& end
	)
{
	// Add this set of points to our JArray
	itsPoints->AppendItem(start);
	itsPoints->AppendItem(end);

	// Tell the widget to redraw itself
	Refresh();

	UndoLine* undo = jnew UndoLine(this);

	itsUndoChain->NewUndo(undo);
}

/******************************************************************************
 RemoveLastLine (private)

	This is called by an UndoLine object.

 ******************************************************************************/

void
UndoWidget::RemoveLastLine()
{
	const JSize count = itsPoints->GetItemCount();
	assert(count >= 2);
	JPoint start = itsPoints->GetItem(count - 1);
	JPoint end = itsPoints->GetItem(count);
	itsPoints->RemoveItem(count);
	itsPoints->RemoveItem(count - 1);
	Refresh();

	RedoLine* redo = jnew RedoLine(this, start, end);

	itsUndoChain->NewUndo(redo);
}
