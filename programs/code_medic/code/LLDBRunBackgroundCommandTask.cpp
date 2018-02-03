/******************************************************************************
 LLDBRunBackgroundCommandTask.cpp

	Let the event loop run between background commands.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBRunBackgroundCommandTask.h"
#include "LLDBLink.h"
#include "cmGlobals.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBRunBackgroundCommandTask::LLDBRunBackgroundCommandTask
	(
	CMCommand* cmd
	)
	:
	itsCmd(cmd)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBRunBackgroundCommandTask::~LLDBRunBackgroundCommandTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
LLDBRunBackgroundCommandTask::Perform()
{
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link != NULL)
		{
		link->SendMedicCommandSync(itsCmd);
		}
}
