/******************************************************************************
 GDeleteMessageTask.cc

	BASE CLASS = JXUrgentTask

	Copyright © 1996 by John Lindal. All rights reserved.

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
