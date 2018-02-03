/******************************************************************************
 LLDBWelcomeTask.h

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_LLDBWelcomeTask
#define _H_LLDBWelcomeTask

#include <JXUrgentTask.h>
#include <JString.h>

class LLDBWelcomeTask : public JXUrgentTask
{
public:

	LLDBWelcomeTask(const JCharacter* msg, const JBoolean restart);

	virtual ~LLDBWelcomeTask();

	virtual void	Perform();

private:

	JString		itsMessage;
	JBoolean	itsRestartFlag;

private:

	// not allowed

	LLDBWelcomeTask(const LLDBWelcomeTask& source);
	const LLDBWelcomeTask& operator=(const LLDBWelcomeTask& source);
};

#endif
