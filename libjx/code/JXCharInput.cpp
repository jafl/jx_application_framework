/******************************************************************************
 JXCharInput.cpp

	BASE CLASS = JXInputField

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXCharInput.h"
#include "JXEditTable.h"
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
	GetText()->SetText(JString(" ", JString::kNoCopy));
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

JUtf8Character
JXCharInput::GetCharacter()
	const
{
	return GetText().GetText().GetFirstCharacter();
}

/******************************************************************************
 SetCharacter

 ******************************************************************************/

void
JXCharInput::SetCharacter
	(
	const JUtf8Character& c
	)
{
	GetText()->SetText(JString(c.GetBytes(), JString::kNoCopy));
	SelectAll();
}

/******************************************************************************
 HandleMouseUp (virtual protected)

 ******************************************************************************/

void
JXCharInput::HandleMouseUp
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JXInputField::HandleMouseUp(pt, button, buttonStates, modifiers);
	SelectAll();
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXCharInput::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JXEditTable* table;
	const bool willDie = GetTable(&table) &&
								   table->WantsInputFieldKey(c, keySym, modifiers);

	SelectAll();
	JXInputField::HandleKeyPress(c, keySym, modifiers);
	if (!willDie)
	{
		SelectAll();
	}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

bool
JXCharInput::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	return false;
}
