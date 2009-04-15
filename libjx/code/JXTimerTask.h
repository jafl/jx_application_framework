/******************************************************************************
 JXTimerTask.h

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTimerTask
#define _H_JXTimerTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>
#include <JBroadcaster.h>

class JXTimerTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXTimerTask(const Time period, const JBoolean oneShot = kJFalse);

	virtual ~JXTimerTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JBoolean itsIsOneShotFlag;

private:

	// not allowed

	JXTimerTask(const JXTimerTask& source);
	const JXTimerTask& operator=(const JXTimerTask& source);

public:

	static const JCharacter* kTimerWentOff;

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
