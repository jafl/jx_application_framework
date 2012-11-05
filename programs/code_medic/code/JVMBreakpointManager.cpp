/******************************************************************************
 JVMBreakpointManager.cpp

	BASE CLASS = CMBreakpointManager

	Copyright © 2009 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
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
	CMBreakpointManager(link, new JVMGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JVMBreakpointManager::~JVMBreakpointManager()
{
}
