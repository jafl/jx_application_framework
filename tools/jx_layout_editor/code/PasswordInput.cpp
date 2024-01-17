/******************************************************************************
 PasswordInput.cpp

	BASE CLASS = InputFieldBase

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "PasswordInput.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PasswordInput::PasswordInput
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

PasswordInput::PasswordInput
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

PasswordInput::~PasswordInput()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
PasswordInput::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("PasswordInput") << std::endl;

	InputFieldBase::StreamOut(output);
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
PasswordInput::GetClassName()
	const
{
	return "JXPasswordInput";
}
