/******************************************************************************
 jXEventUtil.cpp

	Useful functions for dealing with X events.

	Copyright (C) 1996 John Lindal.

 ******************************************************************************/

#include "jXEventUtil.h"
#include "JXDisplay.h"
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 JXGetEventTime

	Return the time stamp of the event.  Returns false if the given
	event doesn't contain a time stamp.

	Selection events contain a time field, but this is a timestamp generated
	by clients, not the current server time.

 ******************************************************************************/

bool
JXGetEventTime
	(
	const XEvent&	xEvent,
	Time*			time
	)
{
	*time = 0;

	if (xEvent.type == KeyPress || xEvent.type == KeyRelease)
		{
		*time = xEvent.xkey.time;
		return true;
		}
	else if (xEvent.type == ButtonPress || xEvent.type == ButtonRelease)
		{
		*time = xEvent.xbutton.time;
		return true;
		}
	else if (xEvent.type == MotionNotify)
		{
		*time = xEvent.xmotion.time;
		return true;
		}
	else if (xEvent.type == EnterNotify || xEvent.type == LeaveNotify)
		{
		*time = xEvent.xcrossing.time;
		return true;
		}
	else if (xEvent.type == PropertyNotify)
		{
		*time = xEvent.xproperty.time;
		return true;
		}
	else
		{
		return false;		// event doesn't contain the information
		}
}

/******************************************************************************
 JXGetMouseLocation

	Return the location of the mouse in root coordinates.
	Returns false if the given event doesn't contain the information.

 ******************************************************************************/

bool
JXGetMouseLocation
	(
	const XEvent&	xEvent,
	JXDisplay*		display,
	JPoint*			pt
	)
{
	pt->Set(0, 0);

	if (xEvent.type == KeyPress || xEvent.type == KeyRelease)
		{
		pt->Set(xEvent.xkey.x_root, xEvent.xkey.y_root);
		return true;
		}
	else if (xEvent.type == ButtonPress || xEvent.type == ButtonRelease)
		{
		pt->Set(xEvent.xbutton.x_root, xEvent.xbutton.y_root);
		return true;
		}
	else if (xEvent.type == MotionNotify)
		{
		pt->Set(xEvent.xmotion.x_root, xEvent.xmotion.y_root);
		return true;
		}
	else if (xEvent.type == EnterNotify || xEvent.type == LeaveNotify)
		{
		pt->Set(xEvent.xcrossing.x_root, xEvent.xcrossing.y_root);
		return true;
		}
	else
		{
		return false;		// event doesn't contain the information
		}
}

/******************************************************************************
 JXGetButtonAndModifierStates

	Return the button and key modifiers states of the event.
	Returns false if the given event doesn't contain the information.

 ******************************************************************************/

bool
JXGetButtonAndModifierStates
	(
	const XEvent&	xEvent,
	JXDisplay*		display,
	unsigned int*	state
	)
{
	*state = 0;

	if (xEvent.type == KeyPress)
		{
		*state = xEvent.xkey.state;
		JIndex modifierIndex;
		if (display->KeycodeToModifier(xEvent.xkey.keycode, &modifierIndex))
			{
			*state = JXKeyModifiers::SetState(display, *state, modifierIndex, true);
			}
		return true;
		}
	else if (xEvent.type == KeyRelease)
		{
		*state = xEvent.xkey.state;
		JIndex modifierIndex;
		if (display->KeycodeToModifier(xEvent.xkey.keycode, &modifierIndex))
			{
			*state = JXKeyModifiers::SetState(display, *state, modifierIndex, false);
			}
		return true;
		}
	else if (xEvent.type == ButtonPress)
		{
		const auto currButton = (JXMouseButton) xEvent.xbutton.button;
		*state = JXButtonStates::SetState(xEvent.xbutton.state,
										  currButton, true);
		return true;
		}
	else if (xEvent.type == ButtonRelease)
		{
		const auto currButton = (JXMouseButton) xEvent.xbutton.button;
		*state = JXButtonStates::SetState(xEvent.xbutton.state,
										  currButton, false);
		return true;
		}
	else if (xEvent.type == MotionNotify)
		{
		*state = xEvent.xmotion.state;
		return true;
		}
	else if (xEvent.type == EnterNotify || xEvent.type == LeaveNotify)
		{
		*state = xEvent.xcrossing.state;
		return true;
		}
	else
		{
		return false;		// event doesn't contain the information
		}
}
