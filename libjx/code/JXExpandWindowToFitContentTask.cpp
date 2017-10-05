/******************************************************************************
 JXExpandWindowToFitContentTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXExpandWindowToFitContentTask.h>
#include <JXWindow.h>
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
