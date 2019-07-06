/******************************************************************************
 XDGetBreakpoints.h

	Copyright (C) 2007 by John Lindal.

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

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	XDGetBreakpoints(const XDGetBreakpoints& source);
	const XDGetBreakpoints& operator=(const XDGetBreakpoints& source);

};

#endif
