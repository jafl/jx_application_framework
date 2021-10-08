/******************************************************************************
 JXAdjustToolBarGeometryTask.h

	Interface for the JXAdjustToolBarGeometryTask class

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXAdjustToolBarGeometryTask
#define _H_JXAdjustToolBarGeometryTask

#include "jx-af/jx/JXUrgentTask.h"

class JXToolBar;

class JXAdjustToolBarGeometryTask : public JXUrgentTask
{
public:

	JXAdjustToolBarGeometryTask(JXToolBar* toolBar);

	~JXAdjustToolBarGeometryTask();

	void	Perform() override;

private:

	JXToolBar*	itsToolBar;
};

#endif
