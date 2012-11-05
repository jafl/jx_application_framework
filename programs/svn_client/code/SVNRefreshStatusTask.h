/******************************************************************************
 SVNRefreshStatusTask.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNRefreshStatusTask
#define _H_SVNRefreshStatusTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>
#include <JBroadcaster.h>

class SVNMainDirector;

class SVNRefreshStatusTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	SVNRefreshStatusTask(SVNMainDirector* directory);

	virtual ~SVNRefreshStatusTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	SVNMainDirector*	itsDirector;	// not owned

private:

	// not allowed

	SVNRefreshStatusTask(const SVNRefreshStatusTask& source);
	const SVNRefreshStatusTask& operator=(const SVNRefreshStatusTask& source);
};

#endif
