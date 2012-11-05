/******************************************************************************
 XDBreakpointManager.h

	Copyright © 2007 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_XDBreakpointManager
#define _H_XDBreakpointManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMBreakpointManager.h"

class XDLink;

class XDBreakpointManager : public CMBreakpointManager
{
public:

	XDBreakpointManager(XDLink* link);

	virtual	~XDBreakpointManager();

private:

	// not allowed

	XDBreakpointManager(const XDBreakpointManager& source);
	const XDBreakpointManager& operator=(const XDBreakpointManager& source);
};

#endif
