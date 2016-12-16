/******************************************************************************
 JBooleanIO.cpp

	Stream operators and utilities for JBoolean type.

	Copyright (C) 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jTypes.h>
#include <jStreamUtil.h>
#include <jAssert.h>

const JCharacter kTrueMarker  = 'T';
const JCharacter kFalseMarker = 'F';

std::istream&
operator>>
	(
	std::istream&	input,
	JBoolean&	jbool
	)
{
	input >> std::ws;

	JCharacter c;
	input.get(c);

	if (c == kTrueMarker)
		{
		jbool = kJTrue;
		}
	else if (c == kFalseMarker)
		{
		jbool = kJFalse;
		}
	else
		{
		input.putback(c);
		JSetState(input, std::ios::failbit);
		}

	return input;
}

std::ostream&
operator<<
	(
	std::ostream&		output,
	const JBoolean	jbool
	)
{
	if (jbool == kJTrue)
		{
		output.put(kTrueMarker);
		}
	else if (jbool == kJFalse)
		{
		output.put(kFalseMarker);
		}
	else
		{
		assert( 0 );	// no other possible values for JBoolean
		}

	return output;
}
