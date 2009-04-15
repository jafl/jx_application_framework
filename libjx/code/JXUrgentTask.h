/******************************************************************************
 JXUrgentTask.h

	Interface for the JXUrgentTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUrgentTask
#define _H_JXUrgentTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JXUrgentTask
{
public:

	JXUrgentTask();

	virtual ~JXUrgentTask();

	virtual void	Perform() = 0;

private:

	// not allowed

	JXUrgentTask(const JXUrgentTask& source);
	const JXUrgentTask& operator=(const JXUrgentTask& source);
};

#endif
