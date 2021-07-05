/******************************************************************************
 CBCmdLineInput.cpp

	BASE CLASS = JXInputField

	Copyright © 1999 by John Lindal.

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
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == kJReturnKey)
		{
		itsDoc->SendText(GetText()->GetText());
		itsDoc->SendText(JString::newline);
		GetText()->SetText(JString::empty);
		}
	else
		{
		JXInputField::HandleKeyPress(c, keySym, modifiers);
		}
}
