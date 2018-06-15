/******************************************************************************
 JXDockWindowTask.cpp

	This is an idle task rather than an urgent task because Gnome takes its
	own sweet time dealing with reparenting.  If we do not wait for the
	window to hide, then it may remain on the desktop!

	BASE CLASS = JXIdleTask, virtual JBroadcaster

	Copyright (C) 2004 by John Lindal.

 ******************************************************************************/

#include "JXDockWindowTask.h"
#include "JXDockWidget.h"
#include "JXTabGroup.h"
#include "JXDisplay.h"
#include "JXWindowDirector.h"
#include "JXWindow.h"
#include "jXGlobals.h"
#include <jAssert.h>

enum
{
	kDeiconifying,
	kUnmapping,	// by waiting for window to be unmapped, we account for network delays
	kReparent,
	kCheckParent,
	kShowWindow	// need separate state for IsDone()
};

const Time kReparentDelay       = 500;	// ms
const Time kCheckParentInterval = 100;	//ms
const Time kShowDelay           = 1;	// ms

static JPtrArray<JXTabGroup>* theUpdateList = jnew JPtrArray<JXTabGroup>(JPtrArrayT::kForgetAll);
static JSize theTaskCount                   = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDockWindowTask::JXDockWindowTask
	(
	JXWindow*		window,
	const Window	parent,
	const JPoint&	topLeft,
	JXDockWidget*	dock
	)
	:
	JXIdleTask(kReparentDelay),
	itsWindow(window),
	itsParent(parent),
	itsPoint(topLeft)
{
	if (itsWindow->IsIconified())
		{
		itsWindow->Deiconify();
		itsState = kDeiconifying;
		}
	else if (itsWindow->IsVisible() && !itsWindow->IsDocked())
		{
		itsWindow->PrivateHide();
		itsState = kUnmapping;
		}
	else
		{
		itsState = kReparent;
		}

	ClearWhenGoingAway(itsWindow, &itsWindow);

	theTaskCount++;

	JXTabGroup* tabGroup = dock->GetTabGroup();
	if (theUpdateList != nullptr && !theUpdateList->Includes(tabGroup))
		{
		theUpdateList->Append(tabGroup);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDockWindowTask::~JXDockWindowTask()
{
	theTaskCount--;
	if (theTaskCount == 0 && theUpdateList != nullptr)
		{
		const JSize count = theUpdateList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(theUpdateList->GetElement(i))->ShowTab(1);
			}

		jdelete theUpdateList;
		theUpdateList = nullptr;
		}
}

/******************************************************************************
 PrepareForDockAll (static)

 ******************************************************************************/

void
JXDockWindowTask::PrepareForDockAll()
{
	if (theUpdateList == nullptr)
		{
		theUpdateList = jnew JPtrArray<JXTabGroup>(JPtrArrayT::kForgetAll);
		assert( theUpdateList != nullptr );
		}
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
	if (itsWindow == nullptr)
		{
		jdelete this;
		return;
		}

	if (itsState == kDeiconifying ||
		itsState == kUnmapping    ||
		!TimeToPerform(delta, maxSleepTime))
		{
		// wait
		}
	else if (itsState == kReparent)
		{
		XReparentWindow(*(itsWindow->GetDisplay()), itsWindow->GetXWindow(),
						itsParent, itsPoint.x, itsPoint.y);
		itsWindow->itsRootChild = None;

		itsState = kCheckParent;
		SetPeriod(kCheckParentInterval);
		}
	else if (itsState == kCheckParent)
		{
		Window root, parent, *childList;
		unsigned int childCount;
		if (!XQueryTree(*(itsWindow->GetDisplay()), itsWindow->GetXWindow(),
						&root, &parent, &childList, &childCount))
			{
			jdelete this;
			return;
			}
		XFree(childList);

		if (parent == itsParent)
			{
			itsState = kShowWindow;
			ResetTimer();
			SetPeriod(kShowDelay);
			}
		else // if (parent == root)
			{
			std::cerr << "XReparentWindow failed during docking:  trying again" << std::endl;
			XReparentWindow(*(itsWindow->GetDisplay()), itsWindow->GetXWindow(),
							itsParent, itsPoint.x, itsPoint.y);
			itsWindow->itsRootChild = None;
			}
		}
	else	// itsState == kShowWindow
		{
		(itsWindow->GetDirector())->Activate();
		jdelete this;
		}
}

/******************************************************************************
 IsDone

 ******************************************************************************/

JBoolean
JXDockWindowTask::IsDone()
	const
{
	return JI2B(itsState == kShowWindow);
}

/******************************************************************************
 Receive (virtual protected)

	Process the given message from the given sender.  This function is not
	pure virtual because not all classes will want to implement it.

 ******************************************************************************/

void
JXDockWindowTask::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsWindow && message.Is(JXWindow::kMapped))
		{
		if (itsState == kDeiconifying)
			{
			itsWindow->PrivateHide();
			itsState = kUnmapping;
			ResetTimer();
			}
		}
	else if (sender == itsWindow && message.Is(JXWindow::kUnmapped))
		{
		if (itsState == kUnmapping)
			{
			itsState = kReparent;
			ResetTimer();
			}
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}
