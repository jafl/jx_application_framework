/******************************************************************************
 JXCloseDirectorTask.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCloseDirectorTask
#define _H_JXCloseDirectorTask

#include "JXIdleTask.h"

class JXDirector;

class JXCloseDirectorTask : public JXIdleTask
{
public:

	static void	Close(JXDirector* director);

	virtual ~JXCloseDirectorTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

protected:

	JXCloseDirectorTask(JXDirector* director);

private:

	JXDirector*	itsDirector;
};

#endif
