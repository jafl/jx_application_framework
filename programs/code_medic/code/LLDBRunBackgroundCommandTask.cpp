/******************************************************************************
 LLDBRunBackgroundCommandTask.cpp

	Let the event loop run between background commands.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBRunBackgroundCommandTask.h"
#include "LLDBLink.h"
#include "CMCommand.h"
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
	ClearWhenGoingAway(itsCmd, &itsCmd);
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
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link != nullptr && itsCmd != nullptr)
		{
		link->SendMedicCommandSync(itsCmd);
		}
}
