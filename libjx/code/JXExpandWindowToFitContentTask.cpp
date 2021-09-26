/******************************************************************************
 JXExpandWindowToFitContentTask.cpp

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXExpandWindowToFitContentTask.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXDialogDirector.h"
#include "jx-af/jx/JXWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExpandWindowToFitContentTask::JXExpandWindowToFitContentTask
	(
	JXWindow* window
	)
	:
	itsWindow(window),
	itShowWindowAfterFTCFlag(false),
	itsFocusWidget(nullptr)
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
	if (itsWindow != nullptr)
	{
		itsWindow->itsExpandTask = nullptr;
		itsWindow->ExpandToFitContent();

		// modal dialogs must be realigned after FTC

		JXWindowDirector* dir = itsWindow->GetDirector();
		auto* dlog = dynamic_cast<JXDialogDirector*>(dir);
		if (dlog != nullptr && dlog->IsModal())
		{
			itsWindow->PlaceAsDialogWindow();
		}

		if (itShowWindowAfterFTCFlag)
		{
			itsWindow->Show();

			if (itsFocusWidget != nullptr)
			{
				itsFocusWidget->Focus();
			}
		}
	}
}
