/******************************************************************************
 JXTimerTask.h

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXTimerTask
#define _H_JXTimerTask

#include "JXIdleTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXTimerTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXTimerTask(const Time period, const bool oneShot = false);

	~JXTimerTask() override;

protected:

	void	Perform(const Time delta) override;

private:

	bool itsIsOneShotFlag;

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
