/******************************************************************************
 XDGetBreakpoints.h

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDGetBreakpoints
#define _H_XDGetBreakpoints

#include "CMGetBreakpoints.h"

class XDGetBreakpoints : public CMGetBreakpoints
{
public:

	XDGetBreakpoints();

	virtual	~XDGetBreakpoints();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	XDGetBreakpoints(const XDGetBreakpoints& source);
	const XDGetBreakpoints& operator=(const XDGetBreakpoints& source);

};

#endif
