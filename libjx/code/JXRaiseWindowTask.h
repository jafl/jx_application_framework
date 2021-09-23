/******************************************************************************
 JXRaiseWindowTask.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRaiseWindowTask
#define _H_JXRaiseWindowTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXWindow;

class JXRaiseWindowTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXRaiseWindowTask(JXWindow* window);

	virtual ~JXRaiseWindowTask();

	virtual void	Perform();

private:

	JXWindow*	itsWindow;	// not owned
};

#endif
