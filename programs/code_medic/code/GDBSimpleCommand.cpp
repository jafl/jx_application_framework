/******************************************************************************
 GDBSimpleCommand.cpp

	Sends a command that does not expect any response.

	BASE CLASS = CMCommand

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBSimpleCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBSimpleCommand::GDBSimpleCommand
	(
	const JCharacter* cmd
	)
	:
	CMCommand(cmd, kJTrue, kJFalse)
{
	ShouldIgnoreResult(kJTrue);
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
