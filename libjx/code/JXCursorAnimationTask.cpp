/******************************************************************************
 JXCursorAnimationTask.cpp

	BASE CLASS = JXIdleTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXCursorAnimationTask.h"
#include "jx-af/jx/JXCursorAnimator.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCursorAnimationTask::JXCursorAnimationTask
	(
	JXCursorAnimator* cursorAnim
	)
	:
	JXIdleTask(JXCursorAnimator::kmsecPerFrame)
{
	itsCursorAnim = cursorAnim;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCursorAnimationTask::~JXCursorAnimationTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXCursorAnimationTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
	{
		itsCursorAnim->NextFrame();
	}
}
