/******************************************************************************
 JXAdjustIWBoundsTask.h

	Interface for the JXAdjustIWBoundsTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAdjustIWBoundsTask
#define _H_JXAdjustIWBoundsTask

#include "jx-af/jx/JXUrgentTask.h"

class JXImageWidget;

class JXAdjustIWBoundsTask : public JXUrgentTask
{
public:

	JXAdjustIWBoundsTask(JXImageWidget* widget);

	~JXAdjustIWBoundsTask();

	void	Perform() override;

private:

	JXImageWidget*	itsImageWidget;		// we don't own this
};

#endif
