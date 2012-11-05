/******************************************************************************
 CMGetFrame.cpp

	BASE CLASS = CMCommand

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMGetFrame.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetFrame::CMGetFrame
	(
	const JCharacter* cmd
	)
	:
	CMCommand(cmd, kJFalse, kJFalse)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetFrame::~CMGetFrame()
{
}
