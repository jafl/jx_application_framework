/******************************************************************************
 CMGetLocalVars.cpp

	BASE CLASS = CMCommand

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMGetLocalVars.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetLocalVars::CMGetLocalVars
	(
	const JCharacter* cmd
	)
	:
	CMCommand(cmd, kJFalse, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetLocalVars::~CMGetLocalVars()
{
}
