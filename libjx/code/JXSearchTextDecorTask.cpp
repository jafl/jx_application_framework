/******************************************************************************
 JXSearchTextDecorTask.cpp

	This creates the horizontal decoration line in JXSearchTextDialog
	*after* FTC, to keep all the checkboxes aligned.

	BASE CLASS = JXUrgentTask, virtual JBroadcaster

	Copyright (C) 2017 by John Lindal.

 ******************************************************************************/

#include "JXSearchTextDecorTask.h"
#include "JXWindow.h"
#include "JXTextCheckbox.h"
#include "JXDownRect.h"
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
	JXUrgentTask(window),
	itsWindow(window),
	itsStayOpenCB(stayOpenCB),
	itsRetainFocusCB(retainFocusCB)
{
}

/******************************************************************************
 Destructor (protected)

 ******************************************************************************/

JXSearchTextDecorTask::~JXSearchTextDecorTask()
{
}

/******************************************************************************
 Perform (virtual protected)

 ******************************************************************************/

void
JXSearchTextDecorTask::Perform()
{
	const JRect soFrame = itsStayOpenCB->GetFrameGlobal();
	const JRect rfFrame = itsRetainFocusCB->GetFrameGlobal();

	auto* line =
		jnew JXDownRect(itsWindow, JXWidget::kFixedLeft, JXWidget::kFixedTop,
						soFrame.left, soFrame.top-6,
						rfFrame.right-soFrame.left, 2);
	assert( line != nullptr );
	line->SetBorderWidth(1);
}
