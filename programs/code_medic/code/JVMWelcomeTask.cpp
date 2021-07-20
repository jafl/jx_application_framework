/******************************************************************************
 JVMWelcomeTask.cpp

	We cannot broadcast the welcome message until everything has been
	created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMWelcomeTask.h"
#include "JVMLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMWelcomeTask::JVMWelcomeTask
	(
	const JString&	msg,
	const bool	error
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
	auto* link = dynamic_cast<JVMLink*>(CMGetLink());
	if (link != nullptr)
		{
		link->BroadcastWelcome(itsMessage, itsErrorFlag);
		}
}
