/******************************************************************************
 JXRaiseWindowTask.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRaiseWindowTask
#define _H_JXRaiseWindowTask

#include "JXUrgentTask.h"

class JXWindow;

class JXRaiseWindowTask : public JXUrgentTask
{
public:

	JXRaiseWindowTask(JXWindow* window);

protected:

	~JXRaiseWindowTask() override;

	void	Perform() override;

private:

	JXWindow*	itsWindow;	// not owned
};

#endif
