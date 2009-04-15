/******************************************************************************
 JXDockWindowTask.h

	Copyright © 2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDockWindowTask
#define _H_JXDockWindowTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JXIdleTask.h"
#include <JBroadcaster.h>
#include <JPoint.h>

class JXWindow;

class JXDockWindowTask : public JXIdleTask, virtual public JBroadcaster
{
public:

	static void	Dock(JXWindow* window, const Window parent, const JPoint& topLeft);

	virtual ~JXDockWindowTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

protected:

	JXDockWindowTask(JXWindow* window, const Window parent, const JPoint& topLeft);

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JXWindow*	itsWindow;
	Window		itsParent;
	JPoint		itsPoint;

private:

	// not allowed

	JXDockWindowTask(const JXDockWindowTask& source);
	const JXDockWindowTask& operator=(const JXDockWindowTask& source);
};

#endif
