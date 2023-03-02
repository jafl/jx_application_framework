/******************************************************************************
 JXUpdateMinSizeTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXUpdateMinSizeTask
#define _H_JXUpdateMinSizeTask

#include "JXUrgentTask.h"

class JXDockWidget;

class JXUpdateMinSizeTask : public JXUrgentTask
{
public:

	JXUpdateMinSizeTask(JXDockWidget* dock);

protected:

	~JXUpdateMinSizeTask() override;

	void	Perform() override;

private:

	JXDockWidget*	itsDockWidget;	// not owned
};

#endif
