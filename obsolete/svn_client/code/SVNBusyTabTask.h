/******************************************************************************
 SVNBusyTabTask.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNBusyTabTask
#define _H_SVNBusyTabTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
