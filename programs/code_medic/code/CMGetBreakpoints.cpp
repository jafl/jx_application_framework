/******************************************************************************
 CMGetBreakpoints.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMGetBreakpoints.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetBreakpoints::CMGetBreakpoints
	(
	const JString& cmd
	)
	:
	CMCommand(cmd, false, false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetBreakpoints::~CMGetBreakpoints()
{
}
