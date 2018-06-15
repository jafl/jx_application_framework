/******************************************************************************
 JXCheckbox.cpp

	BASE CLASS = JXWidget

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXCheckbox.h"
#include "JXWindow.h"
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JXCheckbox::kPushed = "Pushed::JXCheckbox";

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXCheckbox::JXCheckbox
	(
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
	itsIsCheckedFlag = kJFalse;
	itsIsPushedFlag  = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCheckbox::~JXCheckbox()
{
}

/******************************************************************************
 SetState

 ******************************************************************************/

void
JXCheckbox::SetState
	(
	const JBoolean on
	)
{
	if (itsIsCheckedFlag != on)
		{
		itsIsCheckedFlag = on;
		Redraw();
		Broadcast(Pushed(itsIsCheckedFlag));
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXCheckbox::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton)
		{
		itsIsPushedFlag = kJTrue;
		Redraw();
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXCheckbox::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (buttonStates.left())
		{
		const JRect frame     = JXContainer::GlobalToLocal(GetFrameGlobal());
		const JBoolean inside = frame.Contains(pt);
		if (inside && !itsIsPushedFlag)
			{
			itsIsPushedFlag = kJTrue;
			Redraw();
			}
		else if (!inside && itsIsPushedFlag)
			{
			itsIsPushedFlag = kJFalse;
			Redraw();
			}
		}
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXCheckbox::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton && itsIsPushedFlag)
		{
		itsIsPushedFlag = kJFalse;
		ToggleState();
		}
}

/******************************************************************************
 HandleShortcut

	All shortcuts invert the state.

 ******************************************************************************/

void
JXCheckbox::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	SetState(!itsIsCheckedFlag);
}

/******************************************************************************
 SetShortcuts (virtual)

	Derived classes can override to adjust their appearance.

 ******************************************************************************/

void
JXCheckbox::SetShortcuts
	(
	const JString& list
	)
{
	JXWindow* w = GetWindow();
	w->ClearShortcuts(this);
	w->InstallShortcuts(this, list);
	Refresh();
}
