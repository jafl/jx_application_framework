/******************************************************************************
 JVMGetIDSizes.h

	Copyright (C) 2009 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetIDSizes
#define _H_JVMGetIDSizes

#include "CMCommand.h"

class JVMGetIDSizes : public CMCommand
{
public:

	JVMGetIDSizes();

	virtual	~JVMGetIDSizes();

	virtual void	Starting();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	JVMGetIDSizes(const JVMGetIDSizes& source);
	const JVMGetIDSizes& operator=(const JVMGetIDSizes& source);

};

#endif
