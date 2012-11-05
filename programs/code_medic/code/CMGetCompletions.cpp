/******************************************************************************
 CMGetCompletions.cpp

	BASE CLASS = CMCommand

	Copyright © 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMGetCompletions.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetCompletions::CMGetCompletions
	(
	const JString& cmd
	)
	:
	CMCommand(cmd, kJTrue, kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetCompletions::~CMGetCompletions()
{
}
