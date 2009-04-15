/******************************************************************************
 JBooleanIO.cpp

	Stream operators and utilities for JBoolean type.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jTypes.h>
#include <jStreamUtil.h>
#include <jAssert.h>

const JCharacter kTrueMarker  = 'T';
const JCharacter kFalseMarker = 'F';

istream&
operator>>
	(
	istream&	input,
	JBoolean&	jbool
	)
{
	input >> ws;

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
		JSetState(input, ios::failbit);
		}

	return input;
}

ostream&
operator<<
	(
	ostream&		output,
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
