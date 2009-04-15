/******************************************************************************
 DNDWidget.cpp

	BASE CLASS = JXScrollableWidget

	Written by Glenn Bach - 2000. 

 ******************************************************************************/

#include <JXStdInc.h>
#include "DNDWidget.h"
#include "DNDData.h"

#include <JXApplication.h>
#include <JXColormap.h>
#include <JXDisplay.h>
#include <JXDNDManager.h>
#include <JXDragPainter.h>
#include <JXSelectionManager.h>
#include <JXWidget.h>
#include <JXWindowPainter.h>
#include <jXGlobals.h>
#include <sstream>
#include <jAssert.h>

static const JCharacter* kSelectionID = "DNDWidget";

/******************************************************************************
 Constructor

 ******************************************************************************/

DNDWidget::DNDWidget
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

	// See JCollection.h, JOrderedSet.h, and JArray.h for functionality

	itsPoints = new JArray<JPoint>;
	assert( itsPoints != NULL );

	// We need to register the drag X Atom with the display. We will need
	// to keep a copy of the atom to check in WillAcceptDrop.
	itsLinesXAtom	= GetDisplay()->RegisterXAtom(DNDData::GetDNDAtomName());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DNDWidget::~DNDWidget()
{
	// Unlike widgets, which are automatically deleted by the framework,
	// we must delete this JArray since it is a private instance variable.
	delete itsPoints;
}

/******************************************************************************
 Draw (virtual protected)

	This gets called by the event loop every time the Widget needs to
	be redrawn.

 ******************************************************************************/

void
DNDWidget::Draw
	(
	JXWindowPainter& p, 
	const JRect&     rect
	)
{
	// Draw some instructional text
	p.String(10,10,"Shift-drag to initiate Drag-And-Drop.");

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
DNDWidget::HandleMouseDown
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
		// If the shift key is down, we'll start drag-and-drop
		if (modifiers.shift())
			{
			return;
			}
			
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
DNDWidget::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	// Initiate drag-and-drop if the Shift key is pressed.  In this case,
	// we could have done this in the HandleMouseDown().  Normally,
	// however, plain dragging would initiate DND, in which case, one
	// should debounce the drag (i.e. wait for JMouseMoved(itsStartPt, pt)
	// to return kJTrue), and then DND can only be started in
	// HandleMouseDrag().
	if (modifiers.shift())
		{
		// Create the drag data object
		DNDData* data = new DNDData(this, kSelectionID);
		assert(data != NULL);

		// Initiate drag-and-drop with the data object. If this succeeds,
		// HandleMouseDrag will not be called again and you will not get
		// a HandleMouseUp() event.
		BeginDND(pt, buttonStates, modifiers, data);
		return;
		}
		
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

	This gets called by the event loop every time a mouse button is 
	released, unless a drag-and-drop is taking place.

	pt is where the mouse was released, in Bounds coordinates.

	button is the index of the mouse button being pressed.

	buttonStates tells the state of all the mouse buttons.
	Refer to JXButtonStates.h for more information.

	modifiers contains information about modifier keys like Shift and
	Control.  Refer to JXKeyModifiers.h for more information.

 ******************************************************************************/

void
DNDWidget::HandleMouseUp
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
DNDWidget::HandleKeyPress
	(
	const int             key,				
	const JXKeyModifiers& modifiers
	)
{
	// Check if the 'c' key was pressed
	// If so, we want to clear the window
	if (key == 'c')
		{
		// remove all of the points from the JArray
		itsPoints->RemoveAll();
		
		// Redraw 
		Refresh();
		}
		
	// Check if the 'q' key was pressed
	else if (key == 'q')
		{
		// Quit the application if 'q' was pressed
		(JXGetApplication())->Quit();
		}
		
	// If anything else was pressed, pass it up the inheritance tree
	else
		{
		JXScrollableWidget::HandleKeyPress(key,modifiers);
		}
}

/******************************************************************************
 GetSelectionData (protected)

	This is called when DND is terminated by a drop or when the target
	requests the data during a drag, but only if the delayed evaluation
	constructor for JXSelectionData was used.

	id is the string passed to the JXSelectionData constructor.

 ******************************************************************************/

void
DNDWidget::GetSelectionData
	(
	JXSelectionData*	data,
	const JCharacter*	id
	)
{
	// Check to see if this is our id
	if (strcmp(id, kSelectionID) == 0)
		{
		// Cast the data object to the object that we know it is.
		DNDData* lineData = dynamic_cast(DNDData*, data);
		assert(lineData != NULL);

		// Pass our points to the line data object.
		lineData->SetData(*itsPoints);
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Derived classes that accept drops should override this function and
	return kJTrue if they will accept the current drop.  If they return kJFalse,
	they will not receive any DND messages.

	source is non-NULL if the drag is between widgets in the same program.
	This provides a way for compound documents to identify drags between their
	various parts.

 ******************************************************************************/

JBoolean
DNDWidget::WillAcceptDrop
	(
	const JArray<Atom>&		typeList, 
	Atom* action,const Time time, 
	const JXWidget*			source
	)
{
	// There's no point dropping on ourselves in this case.
	if (source == this)
		{
		return kJFalse;
		}

	// Loop through the types and return kJTrue if we find our type.
	const JSize count	= typeList.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		Atom type	= typeList.GetElement(i);
		if (type == itsLinesXAtom)
			{
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget during a drag.

 ******************************************************************************/

void
DNDWidget::HandleDNDHere
	(
	const JPoint& 	pt, 
	const JXWidget*	source
	)
{
	// We're not doing anything here, but typically we would show some type
	// of visible feedback about the current drag.
	// 
	// One neat possibility would be to retrieve the data and show where
	// the lines will appear.
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.
	
 ******************************************************************************/

void
DNDWidget::HandleDNDLeave()
{
	// We're not doing anything here, but we would typically clean up our
	// state at this point, knowing that the drop did not happen.
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

 ******************************************************************************/

void
DNDWidget::HandleDNDDrop
	(
	const JPoint& 		pt, 
	const JArray<Atom>& typeList,
	const Atom 			action, 
	const Time 			time, 
	const JXWidget* 	source
	)
{
	// Here we would typically check what type of action it was, ie. move, 
	// copy, or ask. For simplicity here, we won't deal with that.
	unsigned char* data = NULL;
	JSize dataLength;
	Atom returnType;
	JXSelectionManager::DeleteMethod delMethod;
	
	JXSelectionManager* selManager 	= GetSelectionManager();
	const Atom dndName 				= (GetDNDManager())->GetDNDSelectionName();	

	// Request the appropriate data from the selection manager.
	if (selManager->GetData(dndName, time, itsLinesXAtom,
									 &returnType, &data, &dataLength, &delMethod))
		{
		const std::string s(reinterpret_cast<char*>(data), dataLength);
		std::istringstream is(s);

		// Read in the data according to the format specified in the 
		// data object.
		JSize count;
		is >> count;
		
		for (JIndex i = 1; i <= count; i ++)
			{
			JPoint point;
			is >> point;
			itsPoints->AppendElement(point);
			}
			
		// Tell the widget to redraw itself
		Refresh();

		// Tell the selection manager to delete the data.
		selManager->DeleteData(&data, delMethod);
		}
}
