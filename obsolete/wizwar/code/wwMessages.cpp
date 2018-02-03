/******************************************************************************
 wwMessages.cpp

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "wwMessages.h"
#include <jAssert.h>

/******************************************************************************
 WWMessageType

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&		input,
	WWMessageType&	msg
	)
{
	long temp;
	input >> temp;
	msg = (WWMessageType) temp;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&			output,
	const WWMessageType	msg
	)
{
	output << (long) msg;
	return output;
}

/******************************************************************************
 WWConnectionErrorType

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&				input,
	WWConnectionErrorType&	err
	)
{
	long temp;
	input >> temp;
	err = (WWConnectionErrorType) temp;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&					output,
	const WWConnectionErrorType	err
	)
{
	output << (long) err;
	return output;
}
