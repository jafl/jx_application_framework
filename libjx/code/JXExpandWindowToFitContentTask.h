/******************************************************************************
 JXExpandWindowToFitContentTask.h

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXExpandWindowToFitContentTask
#define _H_JXExpandWindowToFitContentTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXWindow;

class JXExpandWindowToFitContentTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXExpandWindowToFitContentTask(JXWindow* window);

	virtual ~JXExpandWindowToFitContentTask();

	virtual void	Perform();

private:

	JXWindow*	itsWindow;	// not owned

private:

	// not allowed

	JXExpandWindowToFitContentTask(const JXExpandWindowToFitContentTask& source);
	const JXExpandWindowToFitContentTask& operator=(const JXExpandWindowToFitContentTask& source);
};

#endif
