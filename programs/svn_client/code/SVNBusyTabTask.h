/******************************************************************************
 SVNBusyTabTask.h

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNBusyTabTask
#define _H_SVNBusyTabTask

#include <JXIdleTask.h>

class SVNTabGroup;

class SVNBusyTabTask : public JXIdleTask
{
public:

	SVNBusyTabTask(SVNTabGroup* tabGroup);

	virtual ~SVNBusyTabTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	SVNTabGroup*	itsTabGroup;		// owns us

private:

	// not allowed

	SVNBusyTabTask(const SVNBusyTabTask& source);
	const SVNBusyTabTask& operator=(const SVNBusyTabTask& source);
};

#endif
