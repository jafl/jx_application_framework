/******************************************************************************
 JXTEBlinkCaretTask.cpp

	BASE CLASS = JXIdleTask

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXTEBlinkCaretTask.h"
#include "JXTEBase.h"
#include "JXWindow.h"
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
 Perform (virtual protected)

 ******************************************************************************/

void
JXTEBlinkCaretTask::Perform
	(
	const Time delta
	)
{
	if (itsActiveFlag && itsTE->TECaretIsVisible())
	{
		itsTE->TEHideCaret();
	}
	else if (itsActiveFlag)
	{
		itsTE->TEShowCaret();
	}
}
