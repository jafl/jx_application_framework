/******************************************************************************
 CMArrayIndexInput.cpp

	BASE CLASS = JXIntegerInput

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "CMArrayIndexInput.h"
#include <jASCIIConstants.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* CMArrayIndexInput::kReturnKeyPressed =
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
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJReturnKey)
		{
		Broadcast(ReturnKeyPressed());
		}
	else
		{
		JXIntegerInput::HandleKeyPress(key, modifiers);
		}
}
