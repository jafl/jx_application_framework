/******************************************************************************
 JVMArray2DCommand.cpp

	BASE CLASS = CMArray2DCommand

	Copyright � 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "JVMArray2DCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMArray2DCommand::JVMArray2DCommand
	(
	CMArray2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	CMArray2DCommand(dir, table, data)
{
	SetCommand("NOP");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMArray2DCommand::~JVMArray2DCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMArray2DCommand::HandleSuccess
	(
	const JString& data
	)
{
}
