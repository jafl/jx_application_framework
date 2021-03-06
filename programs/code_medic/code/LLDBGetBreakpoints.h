/******************************************************************************
 LLDBGetBreakpoints.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetBreakpoints
#define _H_LLDBGetBreakpoints

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
