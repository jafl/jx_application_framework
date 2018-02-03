/******************************************************************************
 CMVarCommand.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMVarCommand.h"
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* CMVarCommand::kValueUpdated = "CMVarCommand::kValueUpdated";
const JCharacter* CMVarCommand::kValueFailed  = "CMVarCommand::kValueFailed";

/******************************************************************************
 Constructor

 ******************************************************************************/

CMVarCommand::CMVarCommand()
	:
	CMCommand("", kJFalse, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMVarCommand::~CMVarCommand()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMVarCommand::HandleFailure()
{
	Broadcast(ValueMessage(kValueFailed, NULL));
}
