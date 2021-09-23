/******************************************************************************
 JXIdleTask.h

	Interface for the JXIdleTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXIdleTask
#define _H_JXIdleTask

#include <jTypes.h>
#include <X11/Xlib.h>

class JXIdleTask
{
public:

	JXIdleTask(const Time period);

	virtual ~JXIdleTask();

	void	Start();
	void	Stop();

	virtual void	Perform(const Time delta, Time* maxSleepTime) = 0;

	Time	GetPeriod() const;
	void	SetPeriod(const Time period);

	void		ResetTimer();
	bool	TimeToPerform(const Time delta, Time* maxSleepTime);
	bool	CheckIfTimeToPerform(const Time delta);

private:

	Time	itsPeriod;			// time between performances (milliseconds)
	Time	itsElapsedTime;		// time since last performance (milliseconds)

private:

	// not allowed

	JXIdleTask(const JXIdleTask&) = delete;
	JXIdleTask& operator=(const JXIdleTask&) = delete;
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

	Returns true if it is time to perform the task again but does not
	reset the timer.

 ******************************************************************************/

inline bool
JXIdleTask::CheckIfTimeToPerform
	(
	const Time delta
	)
{
	return itsElapsedTime + delta >= itsPeriod;
}

#endif
