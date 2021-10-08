/******************************************************************************
 AnimateWindowIconTask.h

	Interface for the AnimateWindowIconTask class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_AnimateWindowIconTask
#define _H_AnimateWindowIconTask

#include <jx-af/jx/JXIdleTask.h>
#include <jx-af/jcore/JKLRand.h>
#include <jx-af/jcore/JConstBitmap.h>

class JXImage;
class JXWindow;

class AnimateWindowIconTask : public JXIdleTask
{
public:

	AnimateWindowIconTask(JXWindow* window);

	~AnimateWindowIconTask();

	void	Perform(const Time delta, Time* maxSleepTime) override;

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
						   const bool drop);
};

#endif
