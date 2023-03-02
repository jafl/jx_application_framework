/******************************************************************************
 JXScrollTabsTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXScrollTabsTask
#define _H_JXScrollTabsTask

#include "JXUrgentTask.h"

class JXTabGroup;

class JXScrollTabsTask : public JXUrgentTask
{
public:

	JXScrollTabsTask(JXTabGroup* tabGroup);

protected:

	~JXScrollTabsTask() override;

	void	Perform() override;

private:

	JXTabGroup*	itsTabGroup;	// not owned
};

#endif
