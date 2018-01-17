/******************************************************************************
 JXAdjustPSPrintSetupLayoutTask.cpp

	Adjusts location of widgets after fit-to-content.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXAdjustPSPrintSetupLayoutTask.h>
#include <JXDirector.h>
#include <JXWidget.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAdjustPSPrintSetupLayoutTask::JXAdjustPSPrintSetupLayoutTask
	(
	JXDirector*			director,
	const JCoordinate	delta,
	JXWidget*			chooseFileButton,
	JXWidget*			fileInput
	)
	:
	itsDirector(director),
	itsDelta(delta),
	itsChooseFileButton(chooseFileButton),
	itsFileInput(fileInput)
{
	ClearWhenGoingAway(itsDirector, &itsDirector);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAdjustPSPrintSetupLayoutTask::~JXAdjustPSPrintSetupLayoutTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXAdjustPSPrintSetupLayoutTask::Perform()
{
	if (itsDirector != NULL)
		{
		itsChooseFileButton->Move(0, itsDelta);
		itsFileInput->Move(0, itsDelta);
		}
}
