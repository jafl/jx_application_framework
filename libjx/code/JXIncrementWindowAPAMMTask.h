/******************************************************************************
 JXIncrementWindowAPAMMTask.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXIncrementWindowAPAMMTask
#define _H_JXIncrementWindowAPAMMTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class JXWindow;

class JXIncrementWindowAPAMMTask : public JXUrgentTask
{
public:

	JXIncrementWindowAPAMMTask(JXWindow* window);

	virtual ~JXIncrementWindowAPAMMTask();

	virtual void	Perform();

private:

	JXWindow*	itsWindow;		// owns us

private:

	// not allowed

	JXIncrementWindowAPAMMTask(const JXIncrementWindowAPAMMTask& source);
	const JXIncrementWindowAPAMMTask& operator=(const JXIncrementWindowAPAMMTask& source);
};

#endif
