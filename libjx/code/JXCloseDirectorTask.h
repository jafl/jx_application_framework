/******************************************************************************
 JXCloseDirectorTask.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCloseDirectorTask
#define _H_JXCloseDirectorTask

#include "JXUrgentTask.h"

class JXDirector;

class JXCloseDirectorTask : public JXUrgentTask
{
public:

	static void	Close(JXDirector* director);

protected:

	JXCloseDirectorTask(JXDirector* director);

	~JXCloseDirectorTask() override;

	void	Perform() override;

private:

	JXDirector*	itsDirector;
};

#endif
