/******************************************************************************
 CMArrayIndexInput.cpp

	BASE CLASS = JXIntegerInput

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "CMArrayIndexInput.h"
#include <jASCIIConstants.h>
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CMArrayIndexInput::kReturnKeyPressed =
	"ReturnKeyPressed::CMArrayIndexInput";

/******************************************************************************
 Constructor

 ******************************************************************************/

CMArrayIndexInput::CMArrayIndexInput
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
	JXIntegerInput(enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMArrayIndexInput::~CMArrayIndexInput()
{
}

/******************************************************************************
 HandleKeyPress (public)

 ******************************************************************************/

void
CMArrayIndexInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJReturnKey)
		{
		Broadcast(ReturnKeyPressed());
		}
	else
		{
		JXIntegerInput::HandleKeyPress(c, keySym, modifiers);
		}
}
