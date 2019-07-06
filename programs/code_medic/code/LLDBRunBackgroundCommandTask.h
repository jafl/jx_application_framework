/******************************************************************************
 LLDBRunBackgroundCommandTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBRunBackgroundCommandTask
#define _H_LLDBRunBackgroundCommandTask

#include <JXUrgentTask.h>
#include <JBroadcaster.h>

class CMCommand;

class LLDBRunBackgroundCommandTask : public JXUrgentTask, virtual public JBroadcaster
{
public:

	LLDBRunBackgroundCommandTask(CMCommand* cmd);

	virtual ~LLDBRunBackgroundCommandTask();

	virtual void	Perform() override;

private:

	CMCommand*	itsCmd;

private:

	// not allowed

	LLDBRunBackgroundCommandTask(const LLDBRunBackgroundCommandTask& source);
	const LLDBRunBackgroundCommandTask& operator=(const LLDBRunBackgroundCommandTask& source);
};

#endif
