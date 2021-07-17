/******************************************************************************
 JVMVarCommand.cpp

	BASE CLASS = CMVarCommand

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMVarCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMVarCommand::JVMVarCommand
	(
	const JString& origCmd
	)
	:
	CMVarCommand()
{
	SetCommand(JString("NOP", JString::kNoCopy));
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
