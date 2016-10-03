/******************************************************************************
 JVMGetThread.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetThread
#define _H_JVMGetThread

#include "CMGetThread.h"

class JVMGetThread : public CMGetThread
{
public:

	JVMGetThread(CMThreadsWidget* widget);

	virtual	~JVMGetThread();

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	// not allowed

	JVMGetThread(const JVMGetThread& source);
	const JVMGetThread& operator=(const JVMGetThread& source);
};

#endif
