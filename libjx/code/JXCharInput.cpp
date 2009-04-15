/******************************************************************************
 JXCharInput.cpp

	BASE CLASS = JXInputField

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXCharInput.h>
#include <JXEditTable.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCharInput::JXCharInput
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
	SetText(" ");
	SetLengthLimits(1,1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCharInput::~JXCharInput()
{
}

/******************************************************************************
 GetCharacter

	This is safe because the input is invalid if length != 1.

 ******************************************************************************/

JCharacter
JXCharInput::GetCharacter()
	const
{
	return (GetText()).GetCharacter(1);
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXCharInput::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	JXEditTable* table;
	const JBoolean willDie = JI2B( GetTable(&table) &&
								   table->WantsInputFieldKey(key, modifiers) );

	SelectAll();
	JXInputField::HandleKeyPress(key, modifiers);
	if (!willDie)
		{
		SelectAll();
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
JXCharInput::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const Time			time,
	const JXWidget*		source
	)
{
	return kJFalse;
}
