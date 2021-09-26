/******************************************************************************
 JXSearchTextDecorTask.cpp

	This creates the horizontal decoration line in JXSearchTextDialog
	*after* FTC, to keep all the checkboxes aligned.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXSearchTextDecorTask.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXTextCheckbox.h"
#include "jx-af/jx/JXDownRect.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSearchTextDecorTask::JXSearchTextDecorTask
	(
	JXWindow*		window,
	JXTextCheckbox*	stayOpenCB,
	JXTextCheckbox*	retainFocusCB
	)
	:
	itsWindow(window),
	itsStayOpenCB(stayOpenCB),
	itsRetainFocusCB(retainFocusCB)
{
	ClearWhenGoingAway(itsWindow, &itsWindow);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSearchTextDecorTask::~JXSearchTextDecorTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXSearchTextDecorTask::Perform()
{
	if (itsWindow == nullptr)
	{
		return;
	}

	const JRect soFrame = itsStayOpenCB->GetFrameGlobal();
	const JRect rfFrame = itsRetainFocusCB->GetFrameGlobal();

	auto* line =
		jnew JXDownRect(itsWindow, JXWidget::kFixedLeft, JXWidget::kFixedTop,
						soFrame.left, soFrame.top-6,
						rfFrame.right-soFrame.left, 2);
	assert( line != nullptr );
	line->SetBorderWidth(1);
}
