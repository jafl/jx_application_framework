/******************************************************************************
 UndoWidget.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997. 

 ******************************************************************************/

#include <JXStdInc.h>
#include "UndoWidget.h"
#include "UndoLine.h"
#include "RedoLine.h"
#include <JXApplication.h>
#include <JXWidget.h>
#include <JXWindowPainter.h>
#include <JXDragPainter.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jAssert.h>

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
	itsFirstRedoIndex   = 1;
	itsUndoState        = kIdle;

	// This changes our Bounds, independent of what part of us
	// is visible (our Frame).
	SetBounds(500, 400);

	// This array keeps track of the points that define the beginning and
	// ending of each line the is draw in the window.

	// See JCollection.h, JOrderedSet.h, and JArray.h for functionality

	itsPoints = new JArray<JPoint>;
	assert( itsPoints != NULL );

	itsUndoList = new JPtrArray<JUndo>(JPtrArrayT::kDeleteAll);
	assert(itsUndoList != NULL);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

UndoWidget::~UndoWidget()
{
	// Unlike widgets, which are automatically deleted by the framework,
	// we must delete this JArray since it is a private instance variable.
	delete itsPoints;

	// This JPtrArray must also be deleted.
	delete itsUndoList;
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
	p.SetPenColor((GetColormap())->GetBlackColor());
	
	// Find out how many points there are
	// There are count/2 lines
	JSize count = itsPoints->GetElementCount();
	
	// Loop through the points by twos
	for (JSize i = 1; i <= count; i += 2)
		{
		// We need to specify that this is a JPainter function because
		// JXWindowPainter has this function in a different form
		p.JPainter::Line(itsPoints->GetElement(i), itsPoints->GetElement(i+1));
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
	const JBoolean scrolled = ScrollForDrag(pt);

	// Get the drag painter that we created in mouse down
	JPainter* p = NULL;
	const JBoolean ok = GetDragPainter(&p);
	assert( ok );
	
	// Make sure that the left button is pressed, 
	// that we have moved,
	// and that a drag painter exists
	if (buttonStates.left() && pt != itsPrevPt && p != NULL)	// p is NULL for multiple click
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
	// Get the drag painter that we created in mouse down
	JPainter* p = NULL;
	const JBoolean ok = GetDragPainter(&p);
	assert( ok );
	
	// Make sure that the left button is pressed, 
	// and that a drag painter exists
	if (button == kJXLeftButton && p != NULL)	// p is NULL for multiple click
		{
		// Erase the last line that was drawn
		p->Line(itsStartPt, itsPrevPt);
		
		// Delete the drag painter
		DeleteDragPainter();
		
		// Add this set of points to our JArray
		itsPoints->AppendElement(itsStartPt);
		itsPoints->AppendElement(itsPrevPt);

		// Create the undo object to undo the addition of this line

		UndoLine* undo = new UndoLine(this);
		assert(undo != NULL);
		NewUndo(undo);
		
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
	// This can't be called while Undo/Redo is being called.
	assert( itsUndoState == kIdle );

	// See if we have an undo object available.
	JUndo* undo;
	const JBoolean hasUndo = GetCurrentUndo(&undo);

	// Perform the undo.
	if (hasUndo)
		{
		itsUndoState = kUndo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
}

/******************************************************************************
 Redo (public)

 	This is the function that is called when the user asks to redo.

 ******************************************************************************/

void
UndoWidget::Redo()
{
	// This can't be called while Undo/Redo is being called.
	assert( itsUndoState == kIdle );

	// See if we have an redo object available.
	JUndo* undo;
	const JBoolean hasUndo = GetCurrentRedo(&undo);

	// Perform the redo.
	if (hasUndo)
		{
		itsUndoState = kRedo;
		undo->Deactivate();
		undo->Undo();
		itsUndoState = kIdle;
		}
}

/******************************************************************************
 GetCurrentUndo (private)

 ******************************************************************************/

JBoolean
UndoWidget::GetCurrentUndo
	(
	JUndo** undo
	)
	const
{
	if (HasUndo())
		{
		*undo = itsUndoList->NthElement(itsFirstRedoIndex - 1);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 GetCurrentRedo (private)

 ******************************************************************************/

JBoolean
UndoWidget::GetCurrentRedo
	(
	JUndo** redo
	)
	const
{
	if (HasRedo())
		{
		*redo = itsUndoList->NthElement(itsFirstRedoIndex);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 HasUndo (public)

 ******************************************************************************/

JBoolean
UndoWidget::HasUndo()
	const
{
	return JConvertToBoolean(itsFirstRedoIndex > 1);
}

/******************************************************************************
 HasRedo (public)

 ******************************************************************************/

JBoolean
UndoWidget::HasRedo()
	const
{
	return JConvertToBoolean(itsFirstRedoIndex <= itsUndoList->GetElementCount());
}

/******************************************************************************
 NewUndo (private)

 ******************************************************************************/

void
UndoWidget::NewUndo
	(
	JUndo* undo
	)
{
	if (itsUndoList != NULL && itsUndoState == kIdle)
		{
		// clear redo objects

		const JSize undoCount = itsUndoList->GetElementCount();
		for (JIndex i=undoCount; i>=itsFirstRedoIndex; i--)
			{
			itsUndoList->DeleteElement(i);
			}

		// save the new object

		itsUndoList->Append(undo);
		itsFirstRedoIndex++;

		assert( !itsUndoList->IsEmpty() );
		}

	else if (itsUndoList != NULL && itsUndoState == kUndo)
		{
		assert( itsFirstRedoIndex > 1 );

		itsFirstRedoIndex--;
		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);

		undo->SetRedo(kJTrue);
		undo->Deactivate();
		}

	else if (itsUndoList != NULL && itsUndoState == kRedo)
		{
		assert( itsFirstRedoIndex <= itsUndoList->GetElementCount() );

		itsUndoList->SetElement(itsFirstRedoIndex, undo, JPtrArrayT::kDelete);
		itsFirstRedoIndex++;

		undo->SetRedo(kJFalse);
		undo->Deactivate();
		}

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
	itsPoints->AppendElement(start);
	itsPoints->AppendElement(end);
	
	// Tell the widget to redraw itself
	Refresh();	

	UndoLine* undo = new UndoLine(this);
	assert(undo != NULL);

	NewUndo(undo);
}

/******************************************************************************
 RemoveLastLine (private)

 	This is called by an UndoLine object.

 ******************************************************************************/

void
UndoWidget::RemoveLastLine()
{
	const JSize count = itsPoints->GetElementCount();
	assert(count >= 2);
	JPoint start = itsPoints->GetElement(count - 1);
	JPoint end = itsPoints->GetElement(count);
	itsPoints->RemoveElement(count);
	itsPoints->RemoveElement(count - 1);
	Refresh();

	RedoLine* redo = new RedoLine(this, start, end);
	assert(redo != NULL);

	NewUndo(redo);
}

/******************************************************************************
 Instantiate templates

 ******************************************************************************/

#define JTemplateType JUndo
#include <JPtrArray.tmpls>
#undef JTemplateType
