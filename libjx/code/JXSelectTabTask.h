/******************************************************************************
 JXSelectTabTask.h

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXSelectTabTask
#define _H_JXSelectTabTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXTabGroup;

class JXSelectTabTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXSelectTabTask(JXTabGroup* tabGroup, const JIndex index);

	virtual ~JXSelectTabTask();

	virtual void	Perform();

protected:

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JXTabGroup*		itsTabGroup;	// not owned
	JIndex			itsIndex;

private:

	// not allowed

	JXSelectTabTask(const JXSelectTabTask& source);
	const JXSelectTabTask& operator=(const JXSelectTabTask& source);
};

#endif
