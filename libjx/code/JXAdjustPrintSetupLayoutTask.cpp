/******************************************************************************
 JXAdjustPrintSetupLayoutTask.cpp

	Adjusts location of widgets after fit-to-content.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXAdjustPrintSetupLayoutTask.h>
#include <JXDirector.h>
#include <JXWidget.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAdjustPrintSetupLayoutTask::JXAdjustPrintSetupLayoutTask
	(
	JXDirector*	director,
	JXWidget*	printCmd,
	JXWidget*	chooseFileButton,
	JXWidget*	fileInput
	)
	:
	itsDirector(director),
	itsPrintCmd(printCmd),
	itsChooseFileButton(chooseFileButton),
	itsFileInput(fileInput)
{
	ClearWhenGoingAway(itsDirector, &itsDirector);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAdjustPrintSetupLayoutTask::~JXAdjustPrintSetupLayoutTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXAdjustPrintSetupLayoutTask::Perform()
{
	if (itsDirector != NULL)
		{
		const JCoordinate delta =
			itsPrintCmd->GetFrameGlobal().top - itsFileInput->GetFrameGlobal().top;

		itsChooseFileButton->Move(0, delta);
		itsFileInput->Move(0, delta);
		}
}
