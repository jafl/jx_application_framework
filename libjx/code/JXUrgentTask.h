/******************************************************************************
 JXUrgentTask.h

	Interface for the JXUrgentTask class

	Copyright (C) 1996-2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUrgentTask
#define _H_JXUrgentTask

#include <jx-af/jcore/JBroadcaster.h>

class JXUrgentTask : virtual public JBroadcaster
{
	friend class JXApplication;

public:

	JXUrgentTask(JBroadcaster* target);

	void	Go();
	void	Cancel();
	bool	Cancelled() const;

protected:

	virtual ~JXUrgentTask();

	virtual void	Perform() = 0;

private:

	JBroadcaster*	itsTarget;

private:

	// not allowed

	JXUrgentTask(const JXUrgentTask&) = delete;
	JXUrgentTask& operator=(const JXUrgentTask&) = delete;
};


/******************************************************************************
 Cancel

 ******************************************************************************/

inline void
JXUrgentTask::Cancel()
{
	itsTarget = nullptr;
}

/******************************************************************************
 Cancelled

 ******************************************************************************/

inline bool
JXUrgentTask::Cancelled()
	const
{
	return itsTarget == nullptr;
}

#endif
