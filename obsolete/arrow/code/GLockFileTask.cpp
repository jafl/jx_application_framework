/******************************************************************************
 GLockFileTask.cc

	Idle task to update a directory listing.

	BASE CLASS = JXIdleTask

	Copyright (C) 1996 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <GLockFileTask.h>
#include <GMessageTableDir.h>
#include <GMApp.h>

#include <GMGlobals.h>

const Time kLockUpdateDelay = 30000;	// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

GLockFileTask::GLockFileTask
	(
	const JCharacter*	mailbox,
	const JBoolean		iconify
	)
	:
	JXIdleTask(kLockUpdateDelay),
	itsMailbox(mailbox),
	itsIconify(iconify)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLockFileTask::~GLockFileTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
GLockFileTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		if (itsIconify)
			{
			GMGetApplication()->OpenIconifiedMailbox(itsMailbox);
			}
		else
			{
			GMGetApplication()->OpenMailbox(itsMailbox);
			}
		}
}

/******************************************************************************
 GetMailbox

 ******************************************************************************/

const JString&
GLockFileTask::GetMailbox()
{
	return itsMailbox;
}
