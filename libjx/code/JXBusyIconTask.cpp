/******************************************************************************
 JXBusyIconTask.cpp

	BASE CLASS = JXAnimationTask

	Copyright © 1997-2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXBusyIconTask.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jAssert.h>

#include <jx_busy_1_small.xpm>
#include <jx_busy_2_small.xpm>
#include <jx_busy_3_small.xpm>
#include <jx_busy_4_small.xpm>
#include <jx_busy_5_small.xpm>
#include <jx_busy_6_small.xpm>
#include <jx_busy_7_small.xpm>
#include <jx_busy_8_small.xpm>

static JXPM kBusyIcon[] =
{
	jx_busy_1_small, jx_busy_2_small, jx_busy_3_small, jx_busy_4_small,
	jx_busy_5_small, jx_busy_6_small, jx_busy_7_small, jx_busy_8_small
};

const JSize kBusyIconCount = sizeof(kBusyIcon)/sizeof(JXPM);

const JSize kAnimationPeriod = 100;		// milliseconds

/******************************************************************************
 Constructor

 ******************************************************************************/

JXBusyIconTask::JXBusyIconTask
	(
	JXImageWidget* widget
	)
	:
	JXAnimationTask(widget)
{
	itsImageList = new JPtrArray<JXImage>(JPtrArrayT::kDeleteAll, kBusyIconCount);
	assert( itsImageList != NULL );

	JXDisplay* display   = widget->GetDisplay();
	JXColormap* colormap = widget->GetColormap();
	for (JIndex i=1; i<=kBusyIconCount; i++)
		{
		JXImage* icon = new JXImage(display, kBusyIcon[i-1]);
		assert( icon != NULL );
		itsImageList->Append(icon);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXBusyIconTask::~JXBusyIconTask()
{
	delete itsImageList;
}

/******************************************************************************
 GetFrameCount (virtual protected)

 ******************************************************************************/

JSize
JXBusyIconTask::GetFrameCount()
{
	return itsImageList->GetElementCount();
}

/******************************************************************************
 GetFrameTime (virtual protected)

 ******************************************************************************/

void
JXBusyIconTask::GetFrameTime
	(
	const JIndex	frameIndex,
	Time*			tmin,
	Time*			tmax
	)
{
	*tmin = *tmax = kAnimationPeriod;
}

/******************************************************************************
 GetFrame (virtual protected)

 ******************************************************************************/

JXImage*
JXBusyIconTask::GetFrame
	(
	const JIndex frameIndex
	)
{
	return itsImageList->NthElement(frameIndex);
}
