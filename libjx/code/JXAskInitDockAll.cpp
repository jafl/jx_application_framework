/******************************************************************************
 JXAskInitDockAll.cpp

	Asking about "dock all windows" must be done after About dialog is
	closed.  Otherwise, the dock will end up hiding the blocking About
	dialog.

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXAskInitDockAll.h>
#include <JXDockManager.h>
#include <JXDirector.h>
#include <jXGlobals.h>
#include <jAssert.h>

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
	if (itsDirector == NULL)
		{
		if ((JGetUserNotification())->AskUserYes(
					JGetString("StartupTips::JXAskInitDockAll")))
			{
			(JXGetDockManager())->DockAll();
			}

		delete this;
		}
}
