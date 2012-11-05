/******************************************************************************
 GDBBreakpointManager.cpp

	BASE CLASS = CMBreakpointManager

	Copyright © 2007 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include <cmStdInc.h>
#include "GDBBreakpointManager.h"
#include "GDBGetBreakpoints.h"
#include "GDBLink.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBBreakpointManager::GDBBreakpointManager
	(
	GDBLink* link
	)
	:
	CMBreakpointManager(link, new GDBGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBBreakpointManager::~GDBBreakpointManager()
{
}
