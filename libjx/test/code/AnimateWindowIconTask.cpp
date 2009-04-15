/******************************************************************************
 AnimateWindowIconTask.cpp

	BASE CLASS = JXIdleTask

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "AnimateWindowIconTask.h"
#include "WindowIconBitmaps.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXImageMask.h>
#include <JXColormap.h>
#include <JKLRand.h>
#include <jAssert.h>

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
	if (itsState == kSillyIconState)
		{
		DisplayNormalIcon();
		}
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
	const JBoolean perform = TimeToPerform(delta, maxSleepTime);
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

	itsWindow->SetIcon(CreateIcon(kWindowIcon, kWindowIconMask, kJFalse),
					   CreateIcon(kWindowIcon, kWindowIconMask, kJTrue));
}

/******************************************************************************
 DisplaySillyIcon (private)

 ******************************************************************************/

void
AnimateWindowIconTask::DisplaySillyIcon()
{
	itsState = kSillyIconState;

	itsWindow->SetIcon(CreateIcon(kWindowIcon2, kWindowIcon2Mask, kJFalse),
					   CreateIcon(kWindowIcon2, kWindowIcon2Mask, kJTrue));
}

/******************************************************************************
 CreateIcon (private)

 ******************************************************************************/

JXImage*
AnimateWindowIconTask::CreateIcon
	(
	const JConstBitmap&	bitmap,
	const JConstBitmap&	maskBitmap,
	const JBoolean		drop
	)
{
	JXDisplay* display   = itsWindow->GetDisplay();
	JXColormap* colormap = display->GetColormap();

	JXImage* icon = new JXImage(display, colormap, bitmap,
								(drop ? colormap->GetRedColor() : colormap->GetBlueColor()));
	assert( icon != NULL );

	JXImageMask* mask = new JXImageMask(display, colormap, maskBitmap);
	assert( mask != NULL );
	icon->SetMask(mask);

	return icon;
}
