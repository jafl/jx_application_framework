/******************************************************************************
 JXCursorAnimator.h

	Interface for the JXCursorAnimator class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCursorAnimator
#define _H_JXCursorAnimator

#include "jx-af/jx/JXCursor.h"
#include <jx-af/jcore/JArray.h>
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
	bool					itsActiveFlag;
	JArray<JCursorIndex>*	itsFrames;
	JIndex					itsFrameCounter;
	Time					itsLastUpdateTime;

private:

	void	InitFrames(JXDisplay* display);

	// not allowed

	JXCursorAnimator(const JXCursorAnimator&) = delete;
	JXCursorAnimator& operator=(const JXCursorAnimator&) = delete;
};


/******************************************************************************
 Activate

 ******************************************************************************/

inline void
JXCursorAnimator::Activate()
{
	itsActiveFlag = true;
}

/******************************************************************************
 Deactivate

 ******************************************************************************/

inline void
JXCursorAnimator::Deactivate()
{
	itsActiveFlag = false;
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
