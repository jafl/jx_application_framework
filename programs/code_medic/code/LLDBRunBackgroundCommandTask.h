/******************************************************************************
 LLDBRunBackgroundCommandTask.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBRunBackgroundCommandTask
#define _H_LLDBRunBackgroundCommandTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>

class CMCommand;

class LLDBRunBackgroundCommandTask : public JXUrgentTask
{
public:

	LLDBRunBackgroundCommandTask(CMCommand* cmd);

	virtual ~LLDBRunBackgroundCommandTask();

	virtual void	Perform();

private:

	CMCommand*	itsCmd;

private:

	// not allowed

	LLDBRunBackgroundCommandTask(const LLDBRunBackgroundCommandTask& source);
	const LLDBRunBackgroundCommandTask& operator=(const LLDBRunBackgroundCommandTask& source);
};

#endif
