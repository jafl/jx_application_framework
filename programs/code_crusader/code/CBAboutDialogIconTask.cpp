/******************************************************************************
 CBAboutDialogIconTask.cpp

	BASE CLASS = JXAnimationTask

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBAboutDialogIconTask.h"
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jAssert.h>

#include "jcc_about_0.xpm"
#include "jcc_about_1.xpm"
#include "jcc_about_2.xpm"
#include "jcc_about_3.xpm"
#include "jcc_about_4.xpm"

static JXPM kAboutIcon[] =
{
	jcc_about_0, jcc_about_1, jcc_about_2, jcc_about_3, jcc_about_4
};

const JSize kAboutIconCount = sizeof(kAboutIcon)/sizeof(JXPM);

const JSize kMinPausePeriod  = 8000;	// milliseconds
const JSize kMaxPausePeriod  = 10000;	// milliseconds
const JSize kAnimationPeriod = 300;		// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

CBAboutDialogIconTask::CBAboutDialogIconTask
	(
	JXImageWidget* widget
	)
	:
	JXAnimationTask(widget)
{
	itsImageList = jnew JPtrArray<JXImage>(JPtrArrayT::kDeleteAll, kAboutIconCount);
	assert( itsImageList != NULL );

	JXDisplay* display   = widget->GetDisplay();
	JXColormap* colormap = widget->GetColormap();
	for (JIndex i=1; i<=kAboutIconCount; i++)
		{
		JXImage* icon = jnew JXImage(display, kAboutIcon[i-1]);
		assert( icon != NULL );
		itsImageList->Append(icon);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBAboutDialogIconTask::~CBAboutDialogIconTask()
{
	jdelete itsImageList;
}

/******************************************************************************
 GetFrameCount (virtual protected)

 ******************************************************************************/

JSize
CBAboutDialogIconTask::GetFrameCount()
{
	return itsImageList->GetElementCount();
}

/******************************************************************************
 GetFrameTime (virtual protected)

 ******************************************************************************/

void
CBAboutDialogIconTask::GetFrameTime
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
	else
		{
		*tmin = *tmax = kAnimationPeriod;
		}
}

/******************************************************************************
 GetFrame (virtual protected)

 ******************************************************************************/

JXImage*
CBAboutDialogIconTask::GetFrame
	(
	const JIndex frameIndex
	)
{
	return itsImageList->GetElement(frameIndex);
}
