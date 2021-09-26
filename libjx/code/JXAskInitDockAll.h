/******************************************************************************
 JXAskInitDockAll.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAskInitDockAll
#define _H_JXAskInitDockAll

#include "jx-af/jx/JXIdleTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXDirector;

class JXAskInitDockAll : public JXIdleTask, virtual public JBroadcaster
{
public:

	JXAskInitDockAll(JXDirector* dir);

	virtual ~JXAskInitDockAll();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JXDirector*	itsDirector;	// not owned
};

#endif
