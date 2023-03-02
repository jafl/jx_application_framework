/******************************************************************************
 JXIdleTask.h

	Interface for the JXIdleTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXIdleTask
#define _H_JXIdleTask

#include <jx-af/jcore/jTypes.h>
#include <X11/Xlib.h>

class JXIdleTask
{
	friend class JXApplication;

public:

	JXIdleTask(const Time period);

	virtual ~JXIdleTask();

	void	Start();
	void	Stop();

	Time	GetPeriod() const;
	void	SetPeriod(const Time period);

	void	ResetTimer();

protected:

	bool			Ready(const Time delta, Time* maxSleepTime);
	virtual void	Perform(const Time delta) = 0;

private:

	Time	itsPeriod;			// time between runs (milliseconds)
	Time	itsElapsedTime;		// time since last runs (milliseconds)

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

#endif
