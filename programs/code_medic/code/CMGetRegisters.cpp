/******************************************************************************
 CMGetRegisters.cpp

	BASE CLASS = CMCommand

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "CMGetRegisters.h"
#include "CMRegistersDir.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetRegisters::CMGetRegisters
	(
	const JString&	cmd,
	CMRegistersDir*	dir
	)
	:
	CMCommand(cmd, false, true),
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
	itsDir->Update(JString::empty);
}
