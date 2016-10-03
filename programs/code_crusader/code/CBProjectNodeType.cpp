/******************************************************************************
 CBProjectNodeType.cpp

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBProjectNodeType.h"
#include <jAssert.h>

istream&
operator>>
	(
	istream&			input,
	CBProjectNodeType&	type
	)
{
	long temp;
	input >> temp;
	type = (CBProjectNodeType) temp;
	assert( kCBRootNT <= type && type < (long) kCBNTCount );
	return input;
}

ostream&
operator<<
	(
	ostream&				output,
	const CBProjectNodeType	type
	)
{
	output << (long) type;
	return output;
}
