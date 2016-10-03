/******************************************************************************
 GMAboutDialogIconTask.cc

	BASE CLASS = JXAnimationTask

	Copyright (C) 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GMAboutDialogIconTask.h"
#include "GMPixmaps.h"
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jAssert.h>

const JSize kMinPausePeriod	 = 5000;	// milliseconds
const JSize kMaxPausePeriod	 = 8000;	// milliseconds
const JSize kEndPausePeriod	 = 100000;	// milliseconds
const JSize kFastAnimationPeriod = 300;		// milliseconds
const JSize kSlowAnimationPeriod = 500;		// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

GMAboutDialogIconTask::GMAboutDialogIconTask
	(
	JXImageWidget* widget
	)
	:
	JXAnimationTask(widget)
{
	itsImageList = new JPtrArray<JXImage>(JPtrArrayT::kForgetAll, kAboutIconCount);
	assert( itsImageList != NULL );

	JXDisplay* display   = widget->GetDisplay();
	JXColormap* colormap = widget->GetColormap();
	for (JIndex i=1; i<=kAboutIconCount; i++)
		{
		JXImage* icon = new JXImage(display, kAboutIcon[i-1]);
		assert( icon != NULL );
		icon->ConvertToRemoteStorage();

		itsImageList->Append(icon);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMAboutDialogIconTask::~GMAboutDialogIconTask()
{
	itsImageList->DeleteAll();
	delete itsImageList;
}

/******************************************************************************
 GetFrameCount (virtual protected)

 ******************************************************************************/

JSize
GMAboutDialogIconTask::GetFrameCount()
{
	return itsImageList->GetElementCount();
}

/******************************************************************************
 GetFrameTime (virtual protected)

 ******************************************************************************/

void
GMAboutDialogIconTask::GetFrameTime
	(
	const JIndex	frameIndex,
	Time*			tmin,
	Time*			tmax
	)
{
	if (frameIndex == 1)
		{
		*tmin = kMinPausePeriod;
		*tmax = kMaxPausePeriod;
		}
	else if (frameIndex < 4)
		{
		*tmin = *tmax = kFastAnimationPeriod;
		}
	else if (frameIndex == 8)
		{
		*tmin = *tmax = kEndPausePeriod;
		}
	else
		{
		*tmin = *tmax = kSlowAnimationPeriod;
		}
}

/******************************************************************************
 GetFrame (virtual protected)

 ******************************************************************************/

JXImage*
GMAboutDialogIconTask::GetFrame
	(
	const JIndex frameIndex
	)
{
	return itsImageList->NthElement(frameIndex);
}
