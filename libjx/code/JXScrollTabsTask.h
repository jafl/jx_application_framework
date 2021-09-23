/******************************************************************************
 JXScrollTabsTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXScrollTabsTask
#define _H_JXScrollTabsTask

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
};

#endif
