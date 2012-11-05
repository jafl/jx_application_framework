/******************************************************************************
 CMDisplaySourceForMain.cpp

	Finds main() and displays it in the Current Source window.

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMDisplaySourceForMain.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMDisplaySourceForMain::CMDisplaySourceForMain
	(
	CMSourceDirector*	sourceDir,
	const JCharacter*	cmd
	)
	:
	CMCommand(cmd, kJFalse, kJFalse),
	itsSourceDir(sourceDir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMDisplaySourceForMain::~CMDisplaySourceForMain()
{
}
