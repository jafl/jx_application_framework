/******************************************************************************
 CMRunProgramTask.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMRunProgramTask
#define _H_CMRunProgramTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
