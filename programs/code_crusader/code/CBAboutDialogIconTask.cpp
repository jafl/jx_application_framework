/******************************************************************************
 CBAboutDialogIconTask.cpp

	BASE CLASS = JXAnimationTask

	Copyright © 1997 by John Lindal.

 ******************************************************************************/

#include "CBAboutDialogIconTask.h"
#include <JXDisplay.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <JXImageCache.h>
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
	itsImageList = jnew JPtrArray<JXImage>(JPtrArrayT::kForgetAll, kAboutIconCount);
	assert( itsImageList != nullptr );

	JXImageCache* cache = widget->GetDisplay()->GetImageCache();
	for (JIndex i=1; i<=kAboutIconCount; i++)
		{
		itsImageList->Append(cache->GetImage(kAboutIcon[i-1]));
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
