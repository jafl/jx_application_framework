/******************************************************************************
 CMArrayExprInput.cpp

	BASE CLASS = JXInputField

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMArrayExprInput.h"
#include "CMTextDisplayBase.h"
#include <jASCIIConstants.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* CMArrayExprInput::kReturnKeyPressed =
	"ReturnKeyPressed::CMArrayExprInput";

/******************************************************************************
 Constructor

 ******************************************************************************/

CMArrayExprInput::CMArrayExprInput
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
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	CMTextDisplayBase::AdjustFont(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMArrayExprInput::~CMArrayExprInput()
{
}

/******************************************************************************
 HandleKeyPress (public)

 ******************************************************************************/

void
CMArrayExprInput::HandleKeyPress
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
		JXInputField::HandleKeyPress(key, modifiers);
		}
}
