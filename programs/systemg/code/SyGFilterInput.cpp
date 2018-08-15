/******************************************************************************
 SyGFilterInput.cpp

	Input field for entering a wildcard filter.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by Glenn W. Bach.

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
		JXInputField::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 HandleFocusEvent

 ******************************************************************************/

void
SyGFilterInput::HandleFocusEvent()
{
	itsInitialText = GetText()->GetText();
	JXInputField::HandleFocusEvent();
}
