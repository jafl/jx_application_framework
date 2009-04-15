/******************************************************************************
 JXCloseDirectorTask.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCloseDirectorTask
#define _H_JXCloseDirectorTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>

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

private:

	// not allowed

	JXCloseDirectorTask(const JXCloseDirectorTask& source);
	const JXCloseDirectorTask& operator=(const JXCloseDirectorTask& source);
};

#endif
