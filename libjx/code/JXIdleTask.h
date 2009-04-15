/******************************************************************************
 JXIdleTask.h

	Interface for the JXIdleTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXIdleTask
#define _H_JXIdleTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>
#include <X11/Xlib.h>

class JXIdleTask
{
public:

	JXIdleTask(const Time period);

	virtual ~JXIdleTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime) = 0;

	Time	GetPeriod() const;
	void	SetPeriod(const Time period);

	void		ResetTimer();
	JBoolean	TimeToPerform(const Time delta, Time* maxSleepTime);
	JBoolean	CheckIfTimeToPerform(const Time delta);

private:

	Time	itsPeriod;			// time between performances (milliseconds)
	Time	itsElapsedTime;		// time since last performance (milliseconds)

private:

	// not allowed

	JXIdleTask(const JXIdleTask& source);
	const JXIdleTask& operator=(const JXIdleTask& source);
};


/******************************************************************************
 Period

 ******************************************************************************/

inline Time
JXIdleTask::GetPeriod()
	const
{
	return itsPeriod;
}

inline void
JXIdleTask::SetPeriod
	(
	const Time period
	)
{
	itsPeriod = period;
}

/******************************************************************************
 ResetTimer

 ******************************************************************************/

inline void
JXIdleTask::ResetTimer()
{
	itsElapsedTime = 0;
}

/******************************************************************************
 CheckIfTimeToPerform

	Returns kJTrue if it is time to perform the task again but does not
	reset the timer.

 ******************************************************************************/

inline JBoolean
JXIdleTask::CheckIfTimeToPerform
	(
	const Time delta
	)
{
	return JI2B(itsElapsedTime + delta >= itsPeriod);
}

#endif
