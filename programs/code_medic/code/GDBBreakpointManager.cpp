/******************************************************************************
 GDBBreakpointManager.cpp

	BASE CLASS = CMBreakpointManager

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

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
	CMBreakpointManager(link, jnew GDBGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBBreakpointManager::~GDBBreakpointManager()
{
}
