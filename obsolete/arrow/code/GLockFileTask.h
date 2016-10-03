/******************************************************************************
 GLockFileTask.h

	Interface for the GLockFileTask class

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GLockFileTask
#define _H_GLockFileTask

#include <JXIdleTask.h>

#include <JString.h>

#include <jTypes.h>

class GLockFileTask : public JXIdleTask
{
public:

	GLockFileTask(const JCharacter* mailbox, const JBoolean iconify);

	virtual ~GLockFileTask();

	const JString&	GetMailbox();
	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JString		itsMailbox;
	JBoolean	itsIconify;

private:

	// not allowed

	GLockFileTask(const GLockFileTask& source);
	const GLockFileTask& operator=(const GLockFileTask& source);
};

#endif
