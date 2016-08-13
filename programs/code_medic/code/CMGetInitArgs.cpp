/******************************************************************************
 CMGetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .gdbinit.
	Unfortunately, we cannot listen for later calls to "set args" because
	one can only define a hook for single word commands.

	BASE CLASS = CMCommand

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMGetInitArgs.h"
#include <JXInputField.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetInitArgs::CMGetInitArgs
	(
	const JCharacter* cmd
	)
	:
	CMCommand(cmd, kJTrue, kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetInitArgs::~CMGetInitArgs()
{
}
