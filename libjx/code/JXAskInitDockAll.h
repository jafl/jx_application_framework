/******************************************************************************
 JXAskInitDockAll.h

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAskInitDockAll
#define _H_JXAskInitDockAll

#include <JXIdleTask.h>
#include <JBroadcaster.h>

class JXDirector;

class JXAskInitDockAll : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXAskInitDockAll(JXDirector* dir);

	virtual ~JXAskInitDockAll();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JXDirector*	itsDirector;	// not owned

private:

	// not allowed

	JXAskInitDockAll(const JXAskInitDockAll& source);
	const JXAskInitDockAll& operator=(const JXAskInitDockAll& source);
};

#endif
