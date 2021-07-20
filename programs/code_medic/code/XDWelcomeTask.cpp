/******************************************************************************
 XDWelcomeTask.cpp

	We cannot broadcast the Xdebug welcome message until everything has
	been created.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDWelcomeTask.h"
#include "XDLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

XDWelcomeTask::XDWelcomeTask
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

XDWelcomeTask::~XDWelcomeTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
XDWelcomeTask::Perform()
{
	auto* link = dynamic_cast<XDLink*>(CMGetLink());
	if (link != nullptr)
		{
		link->BroadcastWelcome(itsMessage, itsErrorFlag);
		}
}
