/******************************************************************************
 JVMSetProgramTask.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMSetProgramTask
#define _H_JVMSetProgramTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class JVMSetProgramTask : public JXUrgentTask
{
public:

	JVMSetProgramTask();

	virtual ~JVMSetProgramTask();

	virtual void	Perform();

private:

	// not allowed

	JVMSetProgramTask(const JVMSetProgramTask& source);
	const JVMSetProgramTask& operator=(const JVMSetProgramTask& source);
};

#endif
