/******************************************************************************
 CMClearSourceDisplayTask.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMClearSourceDisplayTask
#define _H_CMClearSourceDisplayTask

#include <JXUrgentTask.h>

class CMSourceDirector;

class CMClearSourceDisplayTask : public JXUrgentTask
{
public:

	CMClearSourceDisplayTask(CMSourceDirector* dir);

	virtual ~CMClearSourceDisplayTask();

	virtual void	Perform();

private:

	CMSourceDirector*	itsDirector;	// not owned

private:

	// not allowed

	CMClearSourceDisplayTask(const CMClearSourceDisplayTask& source);
	const CMClearSourceDisplayTask& operator=(const CMClearSourceDisplayTask& source);
};

#endif
