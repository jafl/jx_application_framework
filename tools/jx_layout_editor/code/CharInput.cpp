/******************************************************************************
 CharInput.cpp

	BASE CLASS = InputFieldBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "CharInput.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CharInput::CharInput
	(
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	InputFieldBase(layout, hSizing, vSizing, x,y, w,h)
{
}

CharInput::CharInput
	(
	std::istream&		input,
	const JFileVersion	vers,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	InputFieldBase(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CharInput::~CharInput()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
CharInput::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("CharInput") << std::endl;

	InputFieldBase::StreamOut(output);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
CharInput::GetClassName()
	const
{
	return "JXCharInput";
}
