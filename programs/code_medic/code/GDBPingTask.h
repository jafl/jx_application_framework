/******************************************************************************
 GDBPingTask.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBPingTask
#define _H_GDBPingTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>

class GDBPingTask : public JXIdleTask
{
public:

	GDBPingTask();

	virtual ~GDBPingTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	// not allowed

	GDBPingTask(const GDBPingTask& source);
	const GDBPingTask& operator=(const GDBPingTask& source);
};

#endif
