/******************************************************************************
 GMDirectorTimerTask.cc

	BASE CLASS = public JXTimerTask

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <GMDirectorTimerTask.h>

#include <jAssert.h>

const Time kGMTimerTime	= 1000;

const JCharacter* GMDirectorTimerTask::kDirMenuTimerWentOff = "kDirMenuTimerWentOff::GMDirectorTimerTask";

/******************************************************************************
 Constructor

 *****************************************************************************/

GMDirectorTimerTask::GMDirectorTimerTask
	(
	GMDirectorManager*	mgr,
	GMDirectorMenu*		menu
	)
	:
	JXTimerTask(kGMTimerTime, kJTrue),
	itsMgr(mgr),
	itsMenu(menu)
{
	ListenTo(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMDirectorTimerTask::~GMDirectorTimerTask()
{
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
GMDirectorTimerTask::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXTimerTask::kTimerWentOff))
		{
		Broadcast(DirMenuTimerWentOff(itsMenu));
		}
}
