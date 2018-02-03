/******************************************************************************
 CMGetCompletions.cpp

	BASE CLASS = CMCommand

	Copyright (C) 1998 by Glenn Bach.

 ******************************************************************************/

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
