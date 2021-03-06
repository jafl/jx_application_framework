/******************************************************************************
 XDWelcomeTask.h

	Copyright (C) 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_XDWelcomeTask
#define _H_XDWelcomeTask

#include <JXUrgentTask.h>
#include <JString.h>

class XDWelcomeTask : public JXUrgentTask
{
public:

	XDWelcomeTask(const JCharacter* msg, const JBoolean error);

	virtual ~XDWelcomeTask();

	virtual void	Perform();

private:

	JString		itsMessage;
	JBoolean	itsErrorFlag;

private:

	// not allowed

	XDWelcomeTask(const XDWelcomeTask& source);
	const XDWelcomeTask& operator=(const XDWelcomeTask& source);
};

#endif
