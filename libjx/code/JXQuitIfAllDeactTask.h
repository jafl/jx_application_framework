/******************************************************************************
 JXQuitIfAllDeactTask.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXQuitIfAllDeactTask
#define _H_JXQuitIfAllDeactTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
