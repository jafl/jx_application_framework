/******************************************************************************
 JXSearchTextDecorTask.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSearchTextDecorTask
#define _H_JXSearchTextDecorTask

#include "jx-af/jx/JXUrgentTask.h"
#include <jx-af/jcore/JBroadcaster.h>

class JXWindow;
class JXTextCheckbox;

class JXSearchTextDecorTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	JXSearchTextDecorTask(JXWindow* window,
						  JXTextCheckbox* stayOpenCB, JXTextCheckbox* retainFocusCB);

	~JXSearchTextDecorTask();

	void	Perform() override;

private:

	JXWindow*		itsWindow;			// not owned
	JXTextCheckbox*	itsStayOpenCB;		// not owned
	JXTextCheckbox*	itsRetainFocusCB;	// not owned
};

#endif
