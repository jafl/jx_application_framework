/******************************************************************************
 AnimateHelpMenuTask.cpp

	BASE CLASS = JXIdleTask

	Written by John Lindal.

 ******************************************************************************/

#include "AnimateHelpMenuTask.h"
#include "HelpMenuBitmaps.h"
#include <JXTextMenu.h>
#include <JXImage.h>
#include <jXGlobals.h>
#include <JColorManager.h>
#include <jAssert.h>

const JSize kAnimationPeriod = 400;		// 0.4 second (milliseconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

AnimateHelpMenuTask::AnimateHelpMenuTask
	(
	JXTextMenu*		menu,
	const JIndex	menuItem
	)
	:
	JXIdleTask(kAnimationPeriod),
	itsMenuItem( menuItem )
{
	itsMenu = menu;

	itsImageList = jnew JPtrArray<JXImage>(JPtrArrayT::kDeleteAll, kHelpIconCount);
	assert( itsImageList != nullptr );

	JXDisplay* display   = itsMenu->GetDisplay();
	for (JIndex i=1; i<=kHelpIconCount; i++)
		{
		JColorID fgColor = JColorManager::GetBlackColor();
		if (i == kGreyOutIcon1Index)
			{
			fgColor = JColorManager::GetGrayColor(50);
			}
		else if (i == kGreyOutIcon2Index)
			{
			fgColor = JColorManager::GetGrayColor(75);
			}
		JXImage* icon =
			jnew JXImage(display, kHelpIcon[i-1], fgColor, JColorManager::GetWhiteColor());
		assert( icon != nullptr );
		icon->SetDefaultState(JXImage::kRemoteStorage);
		icon->ConvertToDefaultState();

		itsImageList->Append(icon);
		}

	Reset();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

AnimateHelpMenuTask::~AnimateHelpMenuTask()
{
	jdelete itsImageList;
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
AnimateHelpMenuTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (TimeToPerform(delta, maxSleepTime))
		{
		itsCurrentImage++;
		if (itsCurrentImage > kHelpIconCount)
			{
			itsCurrentImage = 1;
			}

		itsMenu->SetItemImage(itsMenuItem, itsImageList->GetElement(itsCurrentImage),
							  kJFalse);
		}

	if (!itsMenu->IsOpen())
		{
		Stop();
		}
}

/******************************************************************************
 Reset

 ******************************************************************************/

void
AnimateHelpMenuTask::Reset()
{
	itsCurrentImage = 0;
	itsMenu->SetItemImage(itsMenuItem, itsImageList->GetLastElement(), kJFalse);
}
