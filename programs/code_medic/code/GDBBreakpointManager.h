/******************************************************************************
 GDBBreakpointManager.h

	Copyright (C) 2007 by John Lindal.

 *****************************************************************************/

#ifndef _H_GDBBreakpointManager
#define _H_GDBBreakpointManager

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
