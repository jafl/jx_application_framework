/******************************************************************************
 JXTLWAdjustToTreeTask.h

	Interface for the JXTLWAdjustToTreeTask class

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTLWAdjustToTreeTask
#define _H_JXTLWAdjustToTreeTask

#include "JXUrgentTask.h"

class JXTreeListWidget;

class JXTLWAdjustToTreeTask : public JXUrgentTask
{
public:

	JXTLWAdjustToTreeTask(JXTreeListWidget* widget);

	virtual ~JXTLWAdjustToTreeTask();

	virtual void	Perform();

private:

	JXTreeListWidget*	itsWidget;		// we don't own this
};

#endif
