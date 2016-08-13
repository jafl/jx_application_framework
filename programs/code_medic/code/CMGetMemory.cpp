/******************************************************************************
 CMGetMemory.cpp

	BASE CLASS = CMCommand

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CMGetMemory.h"
#include "CMMemoryDir.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CMGetMemory::CMGetMemory
	(
	CMMemoryDir* dir
	)
	:
	CMCommand("", kJFalse, kJTrue),
	itsDir(dir)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMGetMemory::~CMGetMemory()
{
}

/******************************************************************************
 HandleFailure (virtual protected)

 *****************************************************************************/

void
CMGetMemory::HandleFailure()
{
	itsDir->Update("");
}
