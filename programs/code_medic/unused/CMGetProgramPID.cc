/******************************************************************************
 CMGetProgramPID.cc

	BASE CLASS = CMCommand

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMGetProgramPID.h"
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetProgramPID::CMGetProgramPID()
	:
	CMCommand("info prog", kJTrue, kJFalse)
{
	Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetProgramPID::~CMGetProgramPID()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex pidPattern = "(child|attached) (process|Pid|PID) ([[:digit:]]+)";

void
CMGetProgramPID::HandleSuccess
	(
	const JString& data
	)
{
	JArray<JIndexRange> matchList;
	if (pidPattern.Match(data, &matchList))
		{
		
		}
}
