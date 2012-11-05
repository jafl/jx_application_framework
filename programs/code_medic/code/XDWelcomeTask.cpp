/******************************************************************************
 XDWelcomeTask.cpp

	We cannot broadcast the Xdebug welcome message until everything has
	been created.

	BASE CLASS = JXUrgentTask

	Copyright © 2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include "XDWelcomeTask.h"
#include "XDLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

XDWelcomeTask::XDWelcomeTask
	(
	const JCharacter*	msg,
	const JBoolean		error
	)
	:
	itsMessage(msg),
	itsErrorFlag(error)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDWelcomeTask::~XDWelcomeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
XDWelcomeTask::Perform()
{
	XDLink* link = dynamic_cast<XDLink*>(CMGetLink());
	if (link != NULL)
		{
		link->BroadcastWelcome(itsMessage, itsErrorFlag);
		}
}
