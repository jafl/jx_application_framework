/******************************************************************************
 JVMBreakpointManager.cpp

	BASE CLASS = CMBreakpointManager

	Copyright (C) 2009 by John Lindal.

 *****************************************************************************/

#include "JVMBreakpointManager.h"
#include "JVMGetBreakpoints.h"
#include "JVMLink.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JVMBreakpointManager::JVMBreakpointManager
	(
	JVMLink* link
	)
	:
	CMBreakpointManager(link, jnew JVMGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JVMBreakpointManager::~JVMBreakpointManager()
{
}
