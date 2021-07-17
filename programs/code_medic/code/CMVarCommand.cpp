/******************************************************************************
 CMVarCommand.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMVarCommand.h"
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CMVarCommand::kValueUpdated = "CMVarCommand::kValueUpdated";
const JUtf8Byte* CMVarCommand::kValueFailed  = "CMVarCommand::kValueFailed";

/******************************************************************************
 Constructor

 ******************************************************************************/

CMVarCommand::CMVarCommand()
	:
	CMCommand("", false, true)
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
	Broadcast(ValueMessage(kValueFailed, nullptr));
}
