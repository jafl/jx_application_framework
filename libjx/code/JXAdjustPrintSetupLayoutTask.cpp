/******************************************************************************
 JXAdjustPrintSetupLayoutTask.cpp

	Adjusts location of widgets after fit-to-content.

	BASE CLASS = JXUrgentTask

	Copyright (C) 2017-2018 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "JXAdjustPrintSetupLayoutTask.h"
#include "JXDirector.h"
#include "JXWidget.h"

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
	JXUrgentTask(director),
	itsPrintCmd(printCmd),
	itsChooseFileButton(chooseFileButton),
	itsFileInput(fileInput)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXAdjustPrintSetupLayoutTask::~JXAdjustPrintSetupLayoutTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXAdjustPrintSetupLayoutTask::Perform()
{
	const JCoordinate delta =
		itsPrintCmd->GetFrameGlobal().top - itsFileInput->GetFrameGlobal().top;

	itsChooseFileButton->Move(0, delta);
	itsFileInput->Move(0, delta);
}
