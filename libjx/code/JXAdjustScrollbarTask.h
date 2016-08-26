/******************************************************************************
 JXAdjustScrollbarTask.h

	Interface for the JXAdjustScrollbarTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAdjustScrollbarTask
#define _H_JXAdjustScrollbarTask

#include <JXUrgentTask.h>

class JXScrollableWidget;

class JXAdjustScrollbarTask : public JXUrgentTask
{
public:

	JXAdjustScrollbarTask(JXScrollableWidget* widget);

	virtual ~JXAdjustScrollbarTask();

	virtual void	Perform();

private:

	JXScrollableWidget*	itsScrollableWidget;		// we don't own this

private:

	// not allowed

	JXAdjustScrollbarTask(const JXAdjustScrollbarTask& source);
	const JXAdjustScrollbarTask& operator=(const JXAdjustScrollbarTask& source);
};

#endif
