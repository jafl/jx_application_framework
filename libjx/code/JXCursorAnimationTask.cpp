/******************************************************************************
 JXCursorAnimationTask.cpp

	BASE CLASS = JXIdleTask

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXCursorAnimationTask.h>
#include <JXCursorAnimator.h>

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
