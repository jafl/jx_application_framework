/******************************************************************************
 JXAdjustScrollbarTask.h

	Interface for the JXAdjustScrollbarTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXAdjustScrollbarTask
#define _H_JXAdjustScrollbarTask

#include "JXUrgentTask.h"

class JXScrollableWidget;

class JXAdjustScrollbarTask : public JXUrgentTask
{
public:

	JXAdjustScrollbarTask(JXScrollableWidget* widget);

protected:

	~JXAdjustScrollbarTask() override;

	void	Perform() override;

private:

	JXScrollableWidget*	itsScrollableWidget;		// we don't own this
};

#endif
