/******************************************************************************
 XDBreakpointManager.cpp

	BASE CLASS = CMBreakpointManager

	Copyright (C) 2007 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "XDBreakpointManager.h"
#include "XDGetBreakpoints.h"
#include "XDLink.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

XDBreakpointManager::XDBreakpointManager
	(
	XDLink* link
	)
	:
	CMBreakpointManager(link, new XDGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

XDBreakpointManager::~XDBreakpointManager()
{
}
