/******************************************************************************
 JXTEBlinkCaretTask.cpp

	BASE CLASS = JXIdleTask

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXTEBlinkCaretTask.h>
#include <JXTEBase.h>
#include <JXWindow.h>
#include <jAssert.h>

const Time kHalfPeriod = 600;	// 0.6 seconds (milliseconds)

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTEBlinkCaretTask::JXTEBlinkCaretTask
	(
	JXTEBase* te
	)
	:
	JXIdleTask(kHalfPeriod)
{
	itsTE         = te;
	itsActiveFlag = kJFalse;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTEBlinkCaretTask::~JXTEBlinkCaretTask()
{
}

/******************************************************************************
 Reset

 ******************************************************************************/

void
JXTEBlinkCaretTask::Reset()
{
	ResetTimer();
	if (itsTE->IsActive() && itsTE->HasFocus() &&
		(itsTE->GetWindow())->HasFocus())
		{
		itsTE->TEShowCaret();	// hide at first timer rollover
		}
	else
		{
		itsTE->TEHideCaret();
		}
}

/******************************************************************************
 Perform (virtual)

 ******************************************************************************/

void
JXTEBlinkCaretTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (itsActiveFlag && TimeToPerform(delta, maxSleepTime))
		{		
		if (itsTE->TECaretIsVisible())
			{
			itsTE->TEHideCaret();
			}
		else
			{
			itsTE->TEShowCaret();
			}
		}
}
