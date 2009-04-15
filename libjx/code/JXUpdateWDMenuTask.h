/******************************************************************************
 JXUpdateWDMenuTask.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXUpdateWDMenuTask
#define _H_JXUpdateWDMenuTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
