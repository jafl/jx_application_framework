/******************************************************************************
 SyGPathInput.cpp

	Input field for entering a file path.

	BASE CLASS = JXStringInput

	Copyright (C) 1996 by Glenn W. Bach.

 ******************************************************************************/

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
	itsDirList = nullptr;
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

	WantInput(false);
	WantInput(true, WantsTab(), WantsModifiedTab());
}

/******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
SyGPathInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJReturnKey)
		{
		itsDirList->Focus();
		}

	else if (c == kJEscapeKey)
		{
		GetText()->SetText(itsInitialText);
		itsDirList->Focus();
		}

	else
		{
		JXPathInput::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 HandleFocusEvent

 ******************************************************************************/

void
SyGPathInput::HandleFocusEvent()
{
	itsInitialText = GetText()->GetText();
	JXPathInput::HandleFocusEvent();
}
