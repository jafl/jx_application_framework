/******************************************************************************
 JXCursorAnimationTask.h

	Interface for the JXCursorAnimationTask class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCursorAnimationTask
#define _H_JXCursorAnimationTask

#include "jx-af/jx/JXIdleTask.h"

class JXCursorAnimator;

class JXCursorAnimationTask : public JXIdleTask
{
public:

	JXCursorAnimationTask(JXCursorAnimator* cursorAnim);

	virtual ~JXCursorAnimationTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JXCursorAnimator*	itsCursorAnim;		// we don't own this
};

#endif
