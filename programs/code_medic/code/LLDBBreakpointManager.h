/******************************************************************************
 LLDBBreakpointManager.h

	Copyright (C) 2016 by John Lindal.

 *****************************************************************************/

#ifndef _H_LLDBBreakpointManager
#define _H_LLDBBreakpointManager

#include "CMBreakpointManager.h"

class LLDBLink;

class LLDBBreakpointManager : public CMBreakpointManager
{
public:

	LLDBBreakpointManager(LLDBLink* link);

	virtual	~LLDBBreakpointManager();

private:

	// not allowed

	LLDBBreakpointManager(const LLDBBreakpointManager& source);
	const LLDBBreakpointManager& operator=(const LLDBBreakpointManager& source);
};

#endif
