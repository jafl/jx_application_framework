/******************************************************************************
 JXExpandWindowToFitContentTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXExpandWindowToFitContentTask.h>
#include <JXWindow.h>
#include <JXDialogDirector.h>
#include <JXWidget.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExpandWindowToFitContentTask::JXExpandWindowToFitContentTask
	(
	JXWindow* window
	)
	:
	itsWindow(window),
	itShowWindowAfterFTCFlag(kJFalse),
	itsFocusWidget(NULL)
{
	ClearWhenGoingAway(itsWindow, &itsWindow);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXExpandWindowToFitContentTask::~JXExpandWindowToFitContentTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXExpandWindowToFitContentTask::Perform()
{
	if (itsWindow != NULL)
		{
		itsWindow->itsExpandTask = NULL;
		itsWindow->ExpandToFitContent();

		// modal dialogs must be realigned after FTC

		JXWindowDirector* dir = itsWindow->GetDirector();
		JXDialogDirector* dlog = dynamic_cast<JXDialogDirector*>(dir);
		if (dlog != NULL && dlog->IsModal())
			{
			itsWindow->PlaceAsDialogWindow();
			}

		if (itShowWindowAfterFTCFlag)
			{
			itsWindow->Show();

			if (itsFocusWidget != NULL)
				{
				itsFocusWidget->Focus();
				}
			}
		}
}
