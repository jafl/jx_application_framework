/******************************************************************************
 JXSearchTextDecorTask.cpp

	This creates the horizontal decoration line in JXSearchTextDialog
	*after* FTC, to keep all the checkboxes aligned.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include <JXSearchTextDecorTask.h>
#include <JXWindow.h>
#include <JXTextCheckbox.h>
#include <JXDownRect.h>
#include <jAssert.h>

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

	const JRect wFrame  = itsWindow->GetFrameGlobal();
	const JRect soFrame = itsStayOpenCB->GetFrameGlobal();
	const JRect rfFrame = itsRetainFocusCB->GetFrameGlobal();

	JXDownRect* line =
		jnew JXDownRect(itsWindow, JXWidget::kFixedLeft, JXWidget::kFixedTop,
						soFrame.left, soFrame.top-6,
						rfFrame.right-soFrame.left, 2);
	assert( line != nullptr );
	line->SetBorderWidth(1);
}
