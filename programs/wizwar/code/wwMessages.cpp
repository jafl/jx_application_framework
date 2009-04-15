/******************************************************************************
 wwMessages.cpp

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "wwMessages.h"
#include <jAssert.h>

/******************************************************************************
 WWMessageType

 ******************************************************************************/

istream&
operator>>
	(
	istream&		input,
	WWMessageType&	msg
	)
{
	long temp;
	input >> temp;
	msg = (WWMessageType) temp;
	return input;
}

ostream&
operator<<
	(
	ostream&			output,
	const WWMessageType	msg
	)
{
	output << (long) msg;
	return output;
}

/******************************************************************************
 WWConnectionErrorType

 ******************************************************************************/

istream&
operator>>
	(
	istream&				input,
	WWConnectionErrorType&	err
	)
{
	long temp;
	input >> temp;
	err = (WWConnectionErrorType) temp;
	return input;
}

ostream&
operator<<
	(
	ostream&					output,
	const WWConnectionErrorType	err
	)
{
	output << (long) err;
	return output;
}
