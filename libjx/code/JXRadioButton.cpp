/******************************************************************************
 JXRadioButton.cpp

	JXRadioButtons don't broadcast messages because their JXRadioGroup
	does this.

	BASE CLASS = JXWidget

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXRadioButton.h>
#include <JXRadioGroup.h>
#include <JXWindow.h>
#include <jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXRadioButton::JXRadioButton
	(
	const JIndex		id,
	JXRadioGroup*		enclosure,
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
	itsID            = id;
	itsRadioGroup    = enclosure;
	itsIsCheckedFlag = kJFalse;
	itsIsPushedFlag  = kJFalse;

	itsRadioGroup->NewButton(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXRadioButton::~JXRadioButton()
{
}

/******************************************************************************
 Select

 ******************************************************************************/

void
JXRadioButton::Select()
{
	if (!itsIsCheckedFlag)
		{
		itsIsCheckedFlag = kJTrue;
		Redraw();
		itsRadioGroup->NewSelection(this);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXRadioButton::HandleMouseDown
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
JXRadioButton::HandleMouseDrag
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
JXRadioButton::HandleMouseUp
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
		Select();
		}
}

/******************************************************************************
 HandleShortcut

	All shortcuts select us.

 ******************************************************************************/

void
JXRadioButton::HandleShortcut
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	Select();
}

/******************************************************************************
 SetShortcuts (virtual)

	Derived classes can override to adjust their appearance.

 ******************************************************************************/

void
JXRadioButton::SetShortcuts
	(
	const JCharacter* list
	)
{
	JXWindow* w = GetWindow();
	w->ClearShortcuts(this);
	w->InstallShortcuts(this, list);
	Refresh();
}

#define JTemplateType JXRadioButton
#include <JPtrArray.tmpls>
#undef JTemplateType
