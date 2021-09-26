/******************************************************************************
 AnimateWindowIconTask.cpp

	BASE CLASS = JXIdleTask

	Written by John Lindal.

 ******************************************************************************/

#include "AnimateWindowIconTask.h"
#include "WindowIconBitmaps.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXImageMask.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/JKLRand.h>
#include <jx-af/jcore/jAssert.h>

const JSize kSillyIconPeriod = 500;		// 0.5 seconds (milliseconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

AnimateWindowIconTask::AnimateWindowIconTask
	(
	JXWindow* window
	)
	:
	JXIdleTask(0),
	itsRNG()
{
	itsWindow = window;
	DisplayNormalIcon();

	SetPeriod(itsRNG.UniformULong(5,15) * 1000);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

AnimateWindowIconTask::~AnimateWindowIconTask()
{
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
AnimateWindowIconTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	const bool perform = TimeToPerform(delta, maxSleepTime);
	if (perform && itsState == kNormalIconState)
	{
		DisplaySillyIcon();
		SetPeriod(kSillyIconPeriod);
	}
	else if (perform && itsState == kSillyIconState)
	{
		DisplayNormalIcon();
		SetPeriod(itsRNG.UniformULong(5,15) * 1000);
	}
}

/******************************************************************************
 DisplayNormalIcon (private)

 ******************************************************************************/

void
AnimateWindowIconTask::DisplayNormalIcon()
{
	itsState = kNormalIconState;

	itsWindow->SetIcon(CreateIcon(kWindowIcon, kWindowIconMask, false),
					   CreateIcon(kWindowIcon, kWindowIconMask, true));
}

/******************************************************************************
 DisplaySillyIcon (private)

 ******************************************************************************/

void
AnimateWindowIconTask::DisplaySillyIcon()
{
	itsState = kSillyIconState;

	itsWindow->SetIcon(CreateIcon(kWindowIcon2, kWindowIcon2Mask, false),
					   CreateIcon(kWindowIcon2, kWindowIcon2Mask, true));
}

/******************************************************************************
 CreateIcon (private)

 ******************************************************************************/

JXImage*
AnimateWindowIconTask::CreateIcon
	(
	const JConstBitmap&	bitmap,
	const JConstBitmap&	maskBitmap,
	const bool		drop
	)
{
	JXDisplay* display = itsWindow->GetDisplay();

	JXImage* icon = jnew JXImage(display, bitmap,
								(drop ? JColorManager::GetRedColor() : JColorManager::GetBlueColor()));
	assert( icon != nullptr );

	JXImageMask* mask = jnew JXImageMask(display, maskBitmap);
	assert( mask != nullptr );
	icon->SetMask(mask);

	return icon;
}
