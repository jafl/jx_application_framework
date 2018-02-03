/******************************************************************************
 XDBreakpointManager.cpp

	BASE CLASS = CMBreakpointManager

	Copyright (C) 2007 by John Lindal.

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
	CMBreakpointManager(link, jnew XDGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

XDBreakpointManager::~XDBreakpointManager()
{
}
