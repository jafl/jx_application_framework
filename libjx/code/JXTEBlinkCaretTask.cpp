/******************************************************************************
 JXTEBlinkCaretTask.cpp

	BASE CLASS = JXIdleTask

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXTEBlinkCaretTask.h"
#include "jx-af/jx/JXTEBase.h"
#include "jx-af/jx/JXWindow.h"
#include <jx-af/jcore/jAssert.h>

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
	itsActiveFlag = false;
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
