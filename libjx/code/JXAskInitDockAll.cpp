/******************************************************************************
 JXAskInitDockAll.cpp

	Asking about "dock all windows" must be done after About dialog is
	closed.  Otherwise, the dock will end up hiding the blocking About
	dialog.

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXAskInitDockAll.h"
#include "jx-af/jx/JXDockManager.h"
#include "jx-af/jx/JXDirector.h"
#include "jx-af/jx/jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAskInitDockAll::JXAskInitDockAll
	(
	JXDirector* dir
	)
	:
	JXIdleTask(100),
	itsDirector(dir)
{
	ClearWhenGoingAway(itsDirector, &itsDirector);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAskInitDockAll::~JXAskInitDockAll()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXAskInitDockAll::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	if (itsDirector == nullptr)
	{
		if (JGetUserNotification()->AskUserYes(
					JGetString("StartupTips::JXAskInitDockAll")))
		{
			JXGetDockManager()->DockAll();
		}

		jdelete this;
	}
}
