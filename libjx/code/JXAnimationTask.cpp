/******************************************************************************
 JXAnimationTask.cpp

	This class provides the framework for animating a JXImageWidget.
	Derived classes must store the JXImages and implement the following
	functions:

		GetFrameCount
			Returns the number of animation frames.

		GetFrameTime
			Returns the minimum and maximum time (in milliseconds)
			that the frame can be displayed.  A time will be chosen at
			random between these two values.  This is mainly useful for
			animations that should repeat themselves at random intervals.
			For most frames, this function will return min=max so that
			the image is displayed for a fixed amount of time.

		GetFrame
			Returns the JXImage to display.

	For best results, all images should be the same size so JXImageWidget
	doesn't have to change size.

	BASE CLASS = JXIdleTask

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXAnimationTask.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAnimationTask::JXAnimationTask
	(
	JXImageWidget* widget
	)
	:
	JXIdleTask(0),
	itsRNG()
{
	itsWidget       = widget;
	itsCurrentImage = 0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAnimationTask::~JXAnimationTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXAnimationTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		itsCurrentImage++;
		if (itsCurrentImage > GetFrameCount())
			{
			itsCurrentImage = 1;
			}

		Time tmin, tmax;
		GetFrameTime(itsCurrentImage, &tmin, &tmax);
		if (tmin == tmax)
			{
			SetPeriod(tmin);
			}
		else
			{
			SetPeriod(itsRNG.UniformULong(tmin, tmax));
			}

		JXImage* image = GetFrame(itsCurrentImage);
		image->ConvertToRemoteStorage();

		itsWidget->SetImage(image, kJFalse);
		}
}
