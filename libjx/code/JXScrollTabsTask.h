/******************************************************************************
 JXScrollTabsTask.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXScrollTabsTask
#define _H_JXScrollTabsTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXTabGroup;

class JXScrollTabsTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXScrollTabsTask(JXTabGroup* tabGroup);

	virtual ~JXScrollTabsTask();

	virtual void	Perform();

private:

	JXTabGroup*	itsTabGroup;	// not owned

private:

	// not allowed

	JXScrollTabsTask(const JXScrollTabsTask& source);
	const JXScrollTabsTask& operator=(const JXScrollTabsTask& source);
};

#endif
