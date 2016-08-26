/******************************************************************************
 GLClosePlotDirTask.h

	Interface for the GLClosePlotDirTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GLClosePlotDirTask
#define _H_GLClosePlotDirTask

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
