/******************************************************************************
 LLDBRunBackgroundCommandTask.h

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBRunBackgroundCommandTask
#define _H_LLDBRunBackgroundCommandTask

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
