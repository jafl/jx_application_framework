/******************************************************************************
 CMArrayExprInput.cpp

	BASE CLASS = JXInputField

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMArrayExprInput.h"
#include "CMTextDisplayBase.h"
#include <jASCIIConstants.h>
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CMArrayExprInput::kReturnKeyPressed =
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
		JXInputField::HandleKeyPress(c, keySym, modifiers);
		}
}
