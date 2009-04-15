/******************************************************************************
 JXUpdateMinSizeTask.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUpdateMinSizeTask
#define _H_JXUpdateMinSizeTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXDockWidget;

class JXUpdateMinSizeTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXUpdateMinSizeTask(JXDockWidget* dock);

	virtual ~JXUpdateMinSizeTask();

	virtual void	Perform();

protected:

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JXDockWidget*	itsDockWidget;	// not owned

private:

	// not allowed

	JXUpdateMinSizeTask(const JXUpdateMinSizeTask& source);
	const JXUpdateMinSizeTask& operator=(const JXUpdateMinSizeTask& source);
};

#endif
