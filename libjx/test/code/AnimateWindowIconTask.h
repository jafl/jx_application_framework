/******************************************************************************
 AnimateWindowIconTask.h

	Interface for the AnimateWindowIconTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AnimateWindowIconTask
#define _H_AnimateWindowIconTask

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIdleTask.h>
#include <JKLRand.h>
#include <JConstBitmap.h>

class JXImage;
class JXWindow;

class AnimateWindowIconTask : public JXIdleTask
{
public:

	AnimateWindowIconTask(JXWindow* window);

	virtual ~AnimateWindowIconTask();

	virtual void	Perform(const Time delta, Time* maxSleepTime);

private:

	enum State
	{
		kNormalIconState,
		kSillyIconState
	};

private:

	JXWindow*	itsWindow;		// we don't own this
	State		itsState;		// currently displayed icon
	JKLRand		itsRNG;

private:

	void	DisplayNormalIcon();
	void	DisplaySillyIcon();

	JXImage*	CreateIcon(const JConstBitmap& bitmap, const JConstBitmap& mask,
						   const JBoolean drop);

	// not allowed

	AnimateWindowIconTask(const AnimateWindowIconTask& source);
	const AnimateWindowIconTask& operator=(const AnimateWindowIconTask& source);
};

#endif
