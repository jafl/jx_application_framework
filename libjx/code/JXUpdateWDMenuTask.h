/******************************************************************************
 JXUpdateWDMenuTask.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUpdateWDMenuTask
#define _H_JXUpdateWDMenuTask

#include <JXUrgentTask.h>

class JXWDManager;

class JXUpdateWDMenuTask : public JXUrgentTask
{
public:

	JXUpdateWDMenuTask(JXWDManager* wdMgr);

	virtual ~JXUpdateWDMenuTask();

	virtual void	Perform();

private:

	JXWDManager*	itsWDMgr;

private:

	// not allowed

	JXUpdateWDMenuTask(const JXUpdateWDMenuTask& source);
	const JXUpdateWDMenuTask& operator=(const JXUpdateWDMenuTask& source);
};

#endif
