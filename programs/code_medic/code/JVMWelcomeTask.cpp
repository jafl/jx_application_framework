/******************************************************************************
 JVMWelcomeTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright © 2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JVMWelcomeTask.h"
#include "JVMLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMWelcomeTask::JVMWelcomeTask
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

JVMWelcomeTask::~JVMWelcomeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JVMWelcomeTask::Perform()
{
	JVMLink* link = dynamic_cast<JVMLink*>(CMGetLink());
	if (link != NULL)
		{
		link->BroadcastWelcome(itsMessage, itsErrorFlag);
		}
}
