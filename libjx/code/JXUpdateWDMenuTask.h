/******************************************************************************
 JXUpdateWDMenuTask.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUpdateWDMenuTask
#define _H_JXUpdateWDMenuTask

#include "jx-af/jx/JXUrgentTask.h"

class JXWDManager;

class JXUpdateWDMenuTask : public JXUrgentTask
{
public:

	JXUpdateWDMenuTask(JXWDManager* wdMgr);

	~JXUpdateWDMenuTask() override;

	void	Perform() override;

private:

	JXWDManager*	itsWDMgr;
};

#endif
