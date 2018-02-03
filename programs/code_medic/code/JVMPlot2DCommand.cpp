/******************************************************************************
 JVMPlot2DCommand.cpp

	BASE CLASS = CMPlot2DCommand

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMPlot2DCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMPlot2DCommand::JVMPlot2DCommand
	(
	CMPlot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
	:
	CMPlot2DCommand(dir, x, y)
{
	SetCommand("NOP");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMPlot2DCommand::~JVMPlot2DCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMPlot2DCommand::HandleSuccess
	(
	const JString& data
	)
{
}
