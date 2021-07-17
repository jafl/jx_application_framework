/******************************************************************************
 GDBSimpleCommand.cpp

	Sends a command that does not expect any response.

	BASE CLASS = CMCommand

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "GDBSimpleCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBSimpleCommand::GDBSimpleCommand
	(
	const JString& cmd
	)
	:
	CMCommand(cmd, true, false)
{
	ShouldIgnoreResult(true);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBSimpleCommand::~GDBSimpleCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
GDBSimpleCommand::HandleSuccess
	(
	const JString& data
	)
{
}
