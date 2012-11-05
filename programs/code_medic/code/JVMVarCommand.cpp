/******************************************************************************
 JVMVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "JVMVarCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMVarCommand::JVMVarCommand
	(
	const JCharacter* origCmd
	)
	:
	CMVarCommand()
{
	SetCommand("NOP");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMVarCommand::~JVMVarCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMVarCommand::HandleSuccess
	(
	const JString& data
	)
{
}
