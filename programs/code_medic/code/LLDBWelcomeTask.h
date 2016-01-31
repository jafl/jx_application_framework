/******************************************************************************
 LLDBWelcomeTask.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBWelcomeTask
#define _H_LLDBWelcomeTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
