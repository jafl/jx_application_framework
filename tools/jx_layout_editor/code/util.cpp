/******************************************************************************
 util.cpp

	Copyright © 2023 John Lindal.

 ******************************************************************************/

#include "util.h"
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 IsValidIdentifier

 ******************************************************************************/

static const JRegex idPattern("^[_a-z][_a-z0-9]+$", "i");

bool
IsValidIdentifier
	(
	const JString& s
	)
{
	return idPattern.Match(s);
}
