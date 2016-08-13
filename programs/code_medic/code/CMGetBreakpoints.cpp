/******************************************************************************
 CMGetBreakpoints.cpp

	BASE CLASS = CMCommand

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMGetBreakpoints.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetBreakpoints::CMGetBreakpoints
	(
	const JCharacter* cmd
	)
	:
	CMCommand(cmd, kJFalse, kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetBreakpoints::~CMGetBreakpoints()
{
}
