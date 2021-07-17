/******************************************************************************
 CMGetInitArgs.cpp

	Gets the initial setting for the arguments to the program set by .gdbinit.
	Unfortunately, we cannot listen for later calls to "set args" because
	one can only define a hook for single word commands.

	BASE CLASS = CMCommand

	Copyright (C) 2002 by John Lindal.

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
	const JString& cmd
	)
	:
	CMCommand(cmd, true, false)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetInitArgs::~CMGetInitArgs()
{
}
