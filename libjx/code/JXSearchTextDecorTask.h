/******************************************************************************
 JXSearchTextDecorTask.h

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSearchTextDecorTask
#define _H_JXSearchTextDecorTask

#include "JXUrgentTask.h"

class JXWindow;
class JXTextCheckbox;

class JXSearchTextDecorTask : public JXUrgentTask
{
public:

	JXSearchTextDecorTask(JXWindow* window,
						  JXTextCheckbox* stayOpenCB, JXTextCheckbox* retainFocusCB);

protected:

	~JXSearchTextDecorTask() override;

	void	Perform() override;

private:

	JXWindow*		itsWindow;			// not owned
	JXTextCheckbox*	itsStayOpenCB;		// not owned
	JXTextCheckbox*	itsRetainFocusCB;	// not owned
};

#endif
