/******************************************************************************
 GDBBreakpointManager.h

	Copyright © 2007 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GDBBreakpointManager
#define _H_GDBBreakpointManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMBreakpointManager.h"

class GDBLink;

class GDBBreakpointManager : public CMBreakpointManager
{
public:

	GDBBreakpointManager(GDBLink* link);

	virtual	~GDBBreakpointManager();

private:

	// not allowed

	GDBBreakpointManager(const GDBBreakpointManager& source);
	const GDBBreakpointManager& operator=(const GDBBreakpointManager& source);
};

#endif
