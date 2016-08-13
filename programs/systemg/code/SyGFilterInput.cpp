/******************************************************************************
 SyGFilterInput.cpp

	Input field for entering a wildcard filter.

	BASE CLASS = JXInputField

	Copyright © 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <SyGFilterInput.h>
#include <JString.h>
#include <JXWindow.h>
#include <jASCIIConstants.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGFilterInput::SyGFilterInput
	(
	JXWidget*			widget,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsDirList = widget;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGFilterInput::~SyGFilterInput()
{
}

/******************************************************************************
 HandleKeyPress

 ******************************************************************************/

void
SyGFilterInput::HandleKeyPress
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
		JXInputField::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 HandleFocusEvent

 ******************************************************************************/

void
SyGFilterInput::HandleFocusEvent()
{
	itsInitialText = GetText();
	JXInputField::HandleFocusEvent();
}
