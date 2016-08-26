/******************************************************************************
 JXRaiseWindowTask.h

	Copyright © 2010 by John Lindal. All rights reserved.

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

private:

	// not allowed

	JXRaiseWindowTask(const JXRaiseWindowTask& source);
	const JXRaiseWindowTask& operator=(const JXRaiseWindowTask& source);
};

#endif
