/******************************************************************************
 JBooleanIO.cpp

	Adapted from https://codereview.stackexchange.com/questions/14309/conversion-between-enum-and-string-in-c-class-header

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#include "jTypes.h"
#include "jStreamUtil.h"
#include "jAssert.h"

const JUtf8Byte kTrueMarker  = 'T';
const JUtf8Byte kFalseMarker = 'F';

std::ostream&
operator<<
	(
	std::ostream&				output,
	JBoolConstRefHolder const&	data
	)
{
	return output << (data.v ? kTrueMarker : kFalseMarker);
}

std::istream&
operator>>
	(
	std::istream&			input,
	JBoolRefHolder const&	data
	)
{
	input >> std::ws;

	JUtf8Byte c;
	input.get(c);

	if (c == kTrueMarker)
		{
		data.v = true;
		}
	else if (c == kFalseMarker)
		{
		data.v = false;
		}
	else
		{
		input.putback(c);
		JSetState(input, std::ios::failbit);
		}

	return input;
}
