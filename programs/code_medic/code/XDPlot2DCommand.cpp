/******************************************************************************
 XDPlot2DCommand.cpp

	BASE CLASS = CMPlot2DCommand

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDPlot2DCommand.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDPlot2DCommand::XDPlot2DCommand
	(
	CMPlot2DDir*	dir,
	JArray<JFloat>*	x,
	JArray<JFloat>*	y
	)
	:
	CMPlot2DCommand(dir, x, y)
{
	SetCommand("status");
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDPlot2DCommand::~XDPlot2DCommand()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
XDPlot2DCommand::HandleSuccess
	(
	const JString& data
	)
{
}
