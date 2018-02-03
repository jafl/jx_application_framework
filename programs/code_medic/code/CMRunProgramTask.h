/******************************************************************************
 CMRunProgramTask.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMRunProgramTask
#define _H_CMRunProgramTask

#include <JXIdleTask.h>

class CMRunProgramTask : public JXIdleTask
{
public:

	CMRunProgramTask();

	virtual ~CMRunProgramTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	// not allowed

	CMRunProgramTask(const CMRunProgramTask& source);
	const CMRunProgramTask& operator=(const CMRunProgramTask& source);
};

#endif
