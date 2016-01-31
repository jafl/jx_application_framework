/******************************************************************************
 LLDBGetBreakpoints.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetBreakpoints
#define _H_LLDBGetBreakpoints

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetBreakpoints.h"

class LLDBGetBreakpoints : public CMGetBreakpoints
{
public:

	LLDBGetBreakpoints();

	virtual	~LLDBGetBreakpoints();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	LLDBGetBreakpoints(const LLDBGetBreakpoints& source);
	const LLDBGetBreakpoints& operator=(const LLDBGetBreakpoints& source);

};

#endif
