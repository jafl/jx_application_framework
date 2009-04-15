/******************************************************************************
 JXAdjustToolBarGeometryTask.h

	Interface for the JXAdjustToolBarGeometryTask class

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXAdjustToolBarGeometryTask
#define _H_JXAdjustToolBarGeometryTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class JXToolBar;

class JXAdjustToolBarGeometryTask : public JXUrgentTask
{
public:

	JXAdjustToolBarGeometryTask(JXToolBar* toolBar);

	virtual ~JXAdjustToolBarGeometryTask();

	virtual void	Perform();

private:

	JXToolBar*	itsToolBar;

private:

	// not allowed

	JXAdjustToolBarGeometryTask(const JXAdjustToolBarGeometryTask& source);
	const JXAdjustToolBarGeometryTask& operator=(const JXAdjustToolBarGeometryTask& source);
};

#endif
