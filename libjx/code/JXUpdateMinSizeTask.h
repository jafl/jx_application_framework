/******************************************************************************
 JXUpdateMinSizeTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUpdateMinSizeTask
#define _H_JXUpdateMinSizeTask

#include "jx-af/jx/JXUrgentTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXDockWidget;

class JXUpdateMinSizeTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXUpdateMinSizeTask(JXDockWidget* dock);

	~JXUpdateMinSizeTask();

	void	Perform() override;

private:

	JXDockWidget*	itsDockWidget;	// not owned
};

#endif
