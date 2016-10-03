/******************************************************************************
 CBCmdLineInput.cpp

	BASE CLASS = JXInputField

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBCmdLineInput.h"
#include "CBExecOutputDocument.h"
#include <jASCIIConstants.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCmdLineInput::CBCmdLineInput
	(
	CBExecOutputDocument*	doc,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsDoc = doc;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCmdLineInput::~CBCmdLineInput()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
CBCmdLineInput::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == kJReturnKey)
		{
		itsDoc->SendText(GetText());
		itsDoc->SendText("\n");
		SetText("");
		}
	else
		{
		JXInputField::HandleKeyPress(key, modifiers);
		}
}
