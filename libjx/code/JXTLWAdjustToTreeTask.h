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

protected:

	~JXTLWAdjustToTreeTask() override;

	void	Perform() override;

private:

	JXTreeListWidget*	itsWidget;		// not owned
};

#endif
