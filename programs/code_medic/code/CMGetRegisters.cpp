/******************************************************************************
 CMGetRegisters.cpp

	BASE CLASS = CMCommand

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "CMGetRegisters.h"
#include "CMRegistersDir.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetRegisters::CMGetRegisters
	(
	const JCharacter*	cmd,
	CMRegistersDir*		dir
	)
	:
	CMCommand(cmd, kJFalse, kJTrue),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetRegisters::~CMGetRegisters()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMGetRegisters::HandleFailure()
{
	itsDir->Update("");
}
