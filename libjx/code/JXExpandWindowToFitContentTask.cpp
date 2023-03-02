/******************************************************************************
 JXExpandWindowToFitContentTask.cpp

	BASE CLASS = JXUrgentTask

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include "JXExpandWindowToFitContentTask.h"
#include "JXWindow.h"
#include "JXModalDialogDirector.h"
#include "JXWidget.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXExpandWindowToFitContentTask::JXExpandWindowToFitContentTask
	(
	JXWindow* window
	)
	:
	JXUrgentTask(window),
	itsWindow(window),
	itShowWindowAfterFTCFlag(false),
	itsFocusWidget(nullptr)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXExpandWindowToFitContentTask::~JXExpandWindowToFitContentTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXExpandWindowToFitContentTask::Perform()
{
	itsWindow->itsExpandTask = nullptr;
	itsWindow->ExpandToFitContent();

	// modal dialogs must be realigned after FTC

	if (dynamic_cast<JXModalDialogDirector*>(itsWindow->GetDirector()) != nullptr)
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
