/******************************************************************************
 GDBPingTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBPingTask
#define _H_GDBPingTask

#include <JXIdleTask.h>

class GDBPingTask : public JXIdleTask
{
public:

	GDBPingTask();

	virtual ~GDBPingTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime) override;

private:

	// not allowed

	GDBPingTask(const GDBPingTask& source);
	const GDBPingTask& operator=(const GDBPingTask& source);
};

#endif
