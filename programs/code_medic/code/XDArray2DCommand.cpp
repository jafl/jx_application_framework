/******************************************************************************
 XDArray2DCommand.cpp

	BASE CLASS = CMArray2DCommand

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDArray2DCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDArray2DCommand::XDArray2DCommand
	(
	CMArray2DDir*		dir,
	JXStringTable*		table,
	JStringTableData*	data
	)
	:
	CMArray2DCommand(dir, table, data)
{
	SetCommand("status");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDArray2DCommand::~XDArray2DCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDArray2DCommand::HandleSuccess
	(
	const JString& data
	)
{
}
