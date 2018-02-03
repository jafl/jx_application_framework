/******************************************************************************
 GDeleteMessageTask.cc

	BASE CLASS = JXUrgentTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <GDeleteMessageTask.h>
#include <SMTPMessage.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDeleteMessageTask::GDeleteMessageTask
	(
	SMTPMessage* message
	)
{
	itsMessage = message;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDeleteMessageTask::~GDeleteMessageTask()
{
}

/******************************************************************************
 Perform

	By clearing itsAdjustBoundsTask first, we allow the widget
	to create another one, if necessary.

 ******************************************************************************/

void
GDeleteMessageTask::Perform()
{
	delete itsMessage;
}
