/******************************************************************************
 CMDisplaySourceForMain.cpp

	Finds main() and displays it in the Current Source window.

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "CMDisplaySourceForMain.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMDisplaySourceForMain::CMDisplaySourceForMain
	(
	CMSourceDirector*	sourceDir,
	const JString&		cmd
	)
	:
	CMCommand(cmd, false, false),
	itsSourceDir(sourceDir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMDisplaySourceForMain::~CMDisplaySourceForMain()
{
}
