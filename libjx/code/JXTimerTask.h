/******************************************************************************
 JXTimerTask.h

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXTimerTask
#define _H_JXTimerTask

#include "JXIdleTask.h"
#include <JBroadcaster.h>

class JXTimerTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXTimerTask(const Time period, const bool oneShot = false);

	virtual ~JXTimerTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	bool itsIsOneShotFlag;

private:

	// not allowed

	JXTimerTask(const JXTimerTask& source);
	const JXTimerTask& operator=(const JXTimerTask& source);

public:

	static const JUtf8Byte* kTimerWentOff;

	class TimerWentOff: public JBroadcaster::Message
		{
		public:

			TimerWentOff()
				:
				JBroadcaster::Message(kTimerWentOff)
				{ };
		};
};

#endif
