/******************************************************************************
 JXDockWindowTask.cpp

	This is an idle task rather than an urgent task because Gnome takes its
	own sweet time dealing with reparenting.

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright © 2004 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDockWindowTask.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Close (static)

 ******************************************************************************/

void
JXDockWindowTask::Dock
	(
	JXWindow*		window,
	const Window	parent,
	const JPoint&	topLeft
	)
{
	JXDockWindowTask* task = new JXDockWindowTask(window, parent, topLeft);
	assert( task != NULL );
	(JXGetApplication())->InstallIdleTask(task);
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXDockWindowTask::JXDockWindowTask
	(
	JXWindow*		window,
	const Window	parent,
	const JPoint&	topLeft
	)
	:
	JXIdleTask(100),
	itsWindow(window),
	itsParent(parent),
	itsPoint(topLeft)
{
	ListenTo(itsWindow);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockWindowTask::~JXDockWindowTask()
{
}

/******************************************************************************
 Perform

 ******************************************************************************/

void
JXDockWindowTask::Perform
	(
	const Time	delta,
	Time*		maxSleepTime
	)
{
	*maxSleepTime = 100;

	if (itsWindow == NULL)
		{
		delete this;
		return;
		}

	// Gnome takes its own sweet time catching some event related to
	// reparenting, and this leaves the window on the desktop!

	Window root, parent, *childList;
	unsigned int childCount;
	if (!XQueryTree(*(itsWindow->GetDisplay()), itsWindow->GetXWindow(),
					&root, &parent, &childList, &childCount))
		{
		delete this;
		return;
		}
	XFree(childList);

	if (parent == itsParent)
		{
		delete this;
		return;
		}
	else // if (parent == root)
		{
		XReparentWindow(*(itsWindow->GetDisplay()), itsWindow->GetXWindow(),
						itsParent, itsPoint.x, itsPoint.y);
		itsWindow->Raise();
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
JXDockWindowTask::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsWindow)
		{
		itsWindow = NULL;
		}
	else
		{
		JBroadcaster::ReceiveGoingAway(sender);
		}
}
