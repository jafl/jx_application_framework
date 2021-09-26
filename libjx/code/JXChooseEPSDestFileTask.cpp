/******************************************************************************
 JXChooseEPSDestFileTask.cpp

	Requests destination file after fit-to-content.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "jx-af/jx/JXChooseEPSDestFileTask.h"
#include "jx-af/jx/JXEPSPrintSetupDialog.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseEPSDestFileTask::JXChooseEPSDestFileTask
	(
	JXEPSPrintSetupDialog* director
	)
	:
	itsDirector(director)
{
	ClearWhenGoingAway(itsDirector, &itsDirector);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChooseEPSDestFileTask::~JXChooseEPSDestFileTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXChooseEPSDestFileTask::Perform()
{
	if (itsDirector != nullptr)
	{
		itsDirector->ChooseDestinationFile();
	}
}
