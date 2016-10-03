/******************************************************************************
 JXQuitIfAllDeactTask.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXQuitIfAllDeactTask
#define _H_JXQuitIfAllDeactTask

#include <JXIdleTask.h>

class JXQuitIfAllDeactTask : public JXIdleTask
{
public:

	JXQuitIfAllDeactTask();

	virtual ~JXQuitIfAllDeactTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	// not allowed

	JXQuitIfAllDeactTask(const JXQuitIfAllDeactTask& source);
	const JXQuitIfAllDeactTask& operator=(const JXQuitIfAllDeactTask& source);
};

#endif
