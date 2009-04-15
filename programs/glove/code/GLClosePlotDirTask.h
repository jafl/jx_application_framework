/******************************************************************************
 GLClosePlotDirTask.h

	Interface for the GLClosePlotDirTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GLClosePlotDirTask
#define _H_GLClosePlotDirTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class PlotDir;

class GLClosePlotDirTask : public JXUrgentTask
{
public:

	GLClosePlotDirTask(PlotDir* dir);

	virtual ~GLClosePlotDirTask();

	virtual void	Perform();

private:

	PlotDir* 		itsDir;

	// not allowed

	GLClosePlotDirTask(const GLClosePlotDirTask& source);
	const GLClosePlotDirTask& operator=(const GLClosePlotDirTask& source);
};

#endif
