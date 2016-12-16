/******************************************************************************
 SCComponentTypeIO.cpp

	Stream operators and utilities for SCComponentType type.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCComponentType.h"

std::istream&
operator>>
	(
	std::istream&			input,
	SCComponentType&	type
	)
{
	long temp;
	input >> temp;

	// we don't check it here because SCComponent::StreamIn will check it anyway

	type = (SCComponentType) temp;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&				output,
	const SCComponentType	type
	)
{
	output << (long) type;
	return output;
}
