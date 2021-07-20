/******************************************************************************
 LLDBWelcomeTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBWelcomeTask.h"
#include "LLDBLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBWelcomeTask::LLDBWelcomeTask
	(
	const JString&	msg,
	const bool	restart
	)
	:
	itsMessage(msg),
	itsRestartFlag(restart)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBWelcomeTask::~LLDBWelcomeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
LLDBWelcomeTask::Perform()
{
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link != nullptr)
		{
		link->BroadcastWelcome(itsMessage, itsRestartFlag);
		}
}
