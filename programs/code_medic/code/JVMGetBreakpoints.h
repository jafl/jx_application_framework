/******************************************************************************
 JVMGetBreakpoints.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMGetBreakpoints
#define _H_JVMGetBreakpoints

#include "CMGetBreakpoints.h"

class JVMGetBreakpoints : public CMGetBreakpoints
{
public:

	JVMGetBreakpoints();

	virtual	~JVMGetBreakpoints();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMGetBreakpoints(const JVMGetBreakpoints& source);
	const JVMGetBreakpoints& operator=(const JVMGetBreakpoints& source);

};

#endif
