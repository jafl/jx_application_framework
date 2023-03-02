/******************************************************************************
 JXCursorAnimationTask.cpp

	BASE CLASS = JXIdleTask

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXCursorAnimationTask.h"
#include "JXCursorAnimator.h"

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
 Perform (virtual protected)

 ******************************************************************************/

void
JXCursorAnimationTask::Perform
	(
	const Time delta
	)
{
	itsCursorAnim->NextFrame();
}
