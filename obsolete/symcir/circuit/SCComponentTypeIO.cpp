/******************************************************************************
 SCComponentTypeIO.cpp

	Stream operators and utilities for SCComponentType type.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCComponentType.h"

istream&
operator>>
	(
	istream&			input,
	SCComponentType&	type
	)
{
	long temp;
	input >> temp;

	// we don't check it here because SCComponent::StreamIn will check it anyway

	type = (SCComponentType) temp;
	return input;
}

ostream&
operator<<
	(
	ostream&				output,
	const SCComponentType	type
	)
{
	output << (long) type;
	return output;
}
