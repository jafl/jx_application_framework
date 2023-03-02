/******************************************************************************
 JXChooseEPSDestFileTask.cpp

	Requests destination file after fit-to-content.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JXChooseEPSDestFileTask.h"
#include "JXEPSPrintSetupDialog.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseEPSDestFileTask::JXChooseEPSDestFileTask
	(
	JXEPSPrintSetupDialog* director
	)
	:
	JXUrgentTask(director),
	itsDirector(director)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXChooseEPSDestFileTask::~JXChooseEPSDestFileTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXChooseEPSDestFileTask::Perform()
{
	itsDirector->ChooseDestinationFile();
}
