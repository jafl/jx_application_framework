/******************************************************************************
 CMGetBreakpoints.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMGetBreakpoints
#define _H_CMGetBreakpoints

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"

class CMGetBreakpoints : public CMCommand
{
public:

	CMGetBreakpoints(const JCharacter* cmd);

	virtual	~CMGetBreakpoints();

private:

	// not allowed

	CMGetBreakpoints(const CMGetBreakpoints& source);
	const CMGetBreakpoints& operator=(const CMGetBreakpoints& source);
};

#endif
