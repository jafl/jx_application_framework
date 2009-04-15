/******************************************************************************
 AnimateColorTask.cpp

	BASE CLASS = JXIdleTask

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "AnimateColorTask.h"
#include <JXColormap.h>
#include <jAssert.h>

const JSize kAnimationPeriod = 100;		// 0.1 second (milliseconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

AnimateColorTask::AnimateColorTask
	(
	JXColormap*					colormap,
	JArray<JDynamicColorInfo>*	colorList
	)
	:
	JXIdleTask(kAnimationPeriod)
{
	itsColormap       = colormap;
	itsColorList      = colorList;
	itsWavefrontIndex = 1;

	JDynamicColorInfo info = itsColorList->GetElement(1);
	info.color.red   = 65535;
	info.color.green = 0;
	info.color.blue  = 0;
	itsColorList->SetElement(1, info);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

AnimateColorTask::~AnimateColorTask()
{
	delete itsColorList;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
AnimateColorTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		// cycle the colors

		const JSize count = itsColorList->GetElementCount();
		if (itsWavefrontIndex < count)
			{
			itsWavefrontIndex++;
			}

		for (JIndex i=itsWavefrontIndex; i>=2; i--)
			{
			const JDynamicColorInfo info1 = itsColorList->GetElement(i-1);
			JDynamicColorInfo info2       = itsColorList->GetElement(i);
			info2.color = info1.color;
			itsColorList->SetElement(i, info2);
			}

		JDynamicColorInfo info = itsColorList->GetElement(1);
		if (0 < info.color.red && info.color.red < 32768)
			{
			info.color.red   = 0;
			info.color.green = 65535;
			info.color.blue  = 0;
			}
		else if (info.color.red > 0)
			{
			info.color.red -= 4096;
			}
		else if (0 < info.color.green && info.color.green < 32768)
			{
			info.color.red   = 0;
			info.color.green = 0;
			info.color.blue  = 65535;
			}
		else if (info.color.green > 0)
			{
			info.color.green -= 4096;
			}
		else if (0 < info.color.blue && info.color.blue < 32768)
			{
			info.color.red   = 65535;
			info.color.green = 0;
			info.color.blue  = 0;
			}
		else if (info.color.blue > 0)
			{
			info.color.blue -= 4096;
			}
		itsColorList->SetElement(1, info);

		itsColormap->SetDynamicColors(*itsColorList);
		}
}
