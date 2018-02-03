/******************************************************************************
 LLDBBreakpointManager.cpp

	BASE CLASS = CMBreakpointManager

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#include "LLDBBreakpointManager.h"
#include "LLDBGetBreakpoints.h"
#include "LLDBLink.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

LLDBBreakpointManager::LLDBBreakpointManager
	(
	LLDBLink* link
	)
	:
	CMBreakpointManager(link, jnew LLDBGetBreakpoints())
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LLDBBreakpointManager::~LLDBBreakpointManager()
{
}
