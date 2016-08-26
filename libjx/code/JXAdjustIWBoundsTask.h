/******************************************************************************
 JXAdjustIWBoundsTask.h

	Interface for the JXAdjustIWBoundsTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAdjustIWBoundsTask
#define _H_JXAdjustIWBoundsTask

#include <JXUrgentTask.h>

class JXImageWidget;

class JXAdjustIWBoundsTask : public JXUrgentTask
{
public:

	JXAdjustIWBoundsTask(JXImageWidget* widget);

	virtual ~JXAdjustIWBoundsTask();

	virtual void	Perform();

private:

	JXImageWidget*	itsImageWidget;		// we don't own this

private:

	// not allowed

	JXAdjustIWBoundsTask(const JXAdjustIWBoundsTask& source);
	const JXAdjustIWBoundsTask& operator=(const JXAdjustIWBoundsTask& source);
};

#endif
