/******************************************************************************
 JVMBreakpointManager.h

	Copyright © 2007 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JVMBreakpointManager
#define _H_JVMBreakpointManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMBreakpointManager.h"

class JVMLink;
class JVMGetBreakpoints;

class JVMBreakpointManager : public CMBreakpointManager
{
public:

	JVMBreakpointManager(JVMLink* link);

	virtual	~JVMBreakpointManager();

private:

	// not allowed

	JVMBreakpointManager(const JVMBreakpointManager& source);
	const JVMBreakpointManager& operator=(const JVMBreakpointManager& source);
};

#endif
