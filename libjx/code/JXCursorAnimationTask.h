/******************************************************************************
 JXCursorAnimationTask.h

	Interface for the JXCursorAnimationTask class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCursorAnimationTask
#define _H_JXCursorAnimationTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>

class JXCursorAnimator;

class JXCursorAnimationTask : public JXIdleTask
{
public:

	JXCursorAnimationTask(JXCursorAnimator* cursorAnim);

	virtual ~JXCursorAnimationTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	JXCursorAnimator*	itsCursorAnim;		// we don't own this

private:

	// not allowed

	JXCursorAnimationTask(const JXCursorAnimationTask& source);
	const JXCursorAnimationTask& operator=(const JXCursorAnimationTask& source);
};

#endif
