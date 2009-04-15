/******************************************************************************
 JXCursorAnimator.h

	Interface for the JXCursorAnimator class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCursorAnimator
#define _H_JXCursorAnimator

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXCursor.h>
#include <JArray.h>
#include <X11/Xlib.h>

class JXDisplay;
class JXWindow;

class JXCursorAnimator
{
public:

	enum
	{
		kmsecPerFrame = 500		// 0.5 seconds
	};

public:

	JXCursorAnimator(JXWindow* window);

	~JXCursorAnimator();

	void	Activate();
	void	Deactivate();

	void	SetFrameSequence(const JArray<JCursorIndex>& frames);
	void	ResetFrameCounter();
	void	NextFrame();

private:

	JXWindow*				itsWindow;
	JBoolean				itsActiveFlag;
	JArray<JCursorIndex>*	itsFrames;
	JIndex					itsFrameCounter;
	Time					itsLastUpdateTime;

private:

	void	InitFrames(JXDisplay* display);

	// not allowed

	JXCursorAnimator(const JXCursorAnimator& source);
	const JXCursorAnimator& operator=(const JXCursorAnimator& source);
};


/******************************************************************************
 Activate

 ******************************************************************************/

inline void
JXCursorAnimator::Activate()
{
	itsActiveFlag = kJTrue;
}

/******************************************************************************
 Deactivate

 ******************************************************************************/

inline void
JXCursorAnimator::Deactivate()
{
	itsActiveFlag = kJFalse;
}

/******************************************************************************
 ResetFrameCounter

	Reset the counter to start the animation from the first frame.

 ******************************************************************************/

inline void
JXCursorAnimator::ResetFrameCounter()
{
	itsFrameCounter   = 1;
	itsLastUpdateTime = 0;
}

#endif
