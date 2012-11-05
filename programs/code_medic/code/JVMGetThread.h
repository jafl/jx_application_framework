/******************************************************************************
 JVMGetThread.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMGetThread
#define _H_JVMGetThread

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
