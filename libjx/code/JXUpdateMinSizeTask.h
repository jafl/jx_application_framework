/******************************************************************************
 JXUpdateMinSizeTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUpdateMinSizeTask
#define _H_JXUpdateMinSizeTask

#include "JXUrgentTask.h"
#include <JBroadcaster.h>

class JXDockWidget;

class JXUpdateMinSizeTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXUpdateMinSizeTask(JXDockWidget* dock);

	virtual ~JXUpdateMinSizeTask();

	virtual void	Perform();

private:

	JXDockWidget*	itsDockWidget;	// not owned
};

#endif
