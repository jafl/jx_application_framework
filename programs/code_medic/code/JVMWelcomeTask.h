/******************************************************************************
 JVMWelcomeTask.h

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JVMWelcomeTask
#define _H_JVMWelcomeTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXUrgentTask.h>
#include <JString.h>

class JVMWelcomeTask : public JXUrgentTask
{
public:

	JVMWelcomeTask(const JCharacter* msg, const JBoolean error);

	virtual ~JVMWelcomeTask();

	virtual void	Perform();

private:

	JString		itsMessage;
	JBoolean	itsErrorFlag;

private:

	// not allowed

	JVMWelcomeTask(const JVMWelcomeTask& source);
	const JVMWelcomeTask& operator=(const JVMWelcomeTask& source);
};

#endif
