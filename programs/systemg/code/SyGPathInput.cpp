/******************************************************************************
 SyGPathInput.cpp

	Input field for entering a file path.

	BASE CLASS = JXStringInput

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include <SyGPathInput.h>
#include <JString.h>
#include <JXWindow.h>
#include <jASCIIConstants.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGPathInput::SyGPathInput
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
	JXPathInput(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsDirList = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGPathInput::~SyGPathInput()
{
}

/******************************************************************************
 SetDirList

	We are created before the table so we can't get the pointer
	in our constructor.

 ******************************************************************************/

void
SyGPathInput::SetDirList
	(
	JXWidget* dirList
	)
{
	itsDirList = dirList;

	// this re-orders the list so dir list gets focus

	WantInput(kJFalse);
	WantInput(kJTrue, WantsTab(), WantsModifiedTab());
}

/******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
SyGPathInput::HandleKeyPress
	(
	const int key,
	const JXKeyModifiers&   modifiers
	)
{
	if (key == kJReturnKey)
		{
		itsDirList->Focus();
		}

	else if (key == kJEscapeKey)
		{
		SetText(itsInitialText);
		itsDirList->Focus();
		}

	else
		{
		JXPathInput::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 HandleFocusEvent

 ******************************************************************************/

void
SyGPathInput::HandleFocusEvent()
{
	itsInitialText = GetText();
	JXPathInput::HandleFocusEvent();
}
