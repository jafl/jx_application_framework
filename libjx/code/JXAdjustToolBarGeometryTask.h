/******************************************************************************
 JXAdjustToolBarGeometryTask.h

	Interface for the JXAdjustToolBarGeometryTask class

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXAdjustToolBarGeometryTask
#define _H_JXAdjustToolBarGeometryTask

#include "JXUrgentTask.h"

class JXToolBar;

class JXAdjustToolBarGeometryTask : public JXUrgentTask
{
public:

	JXAdjustToolBarGeometryTask(JXToolBar* toolBar);

	virtual ~JXAdjustToolBarGeometryTask();

	virtual void	Perform();

private:

	JXToolBar*	itsToolBar;
};

#endif
