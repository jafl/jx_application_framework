/******************************************************************************
 JXUrgentTask.h

	Interface for the JXUrgentTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUrgentTask
#define _H_JXUrgentTask

#include <jx-af/jcore/jTypes.h>

class JXUrgentTask
{
public:

	JXUrgentTask();

	virtual ~JXUrgentTask();

	void	Go();

	virtual void	Perform() = 0;

private:

	// not allowed

	JXUrgentTask(const JXUrgentTask&) = delete;
	JXUrgentTask& operator=(const JXUrgentTask&) = delete;
};

#endif
