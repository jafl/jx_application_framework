/******************************************************************************
 JXApplication.cpp

	Initializes global objects and manages the event loop.

	Urgent tasks are performed after the current event and are then deleted.

	Idle tasks are performed whenever the event loop is idle.  It is safe
	to delete a JXIdleTask object because it will automatically remove itself
	from the task list.

	BASE CLASS = JXDirector

	Copyright (C) 1996-97 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXApplication.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXIdleTask.h"
#include "jx-af/jx/JXQuitIfAllDeactTask.h"
#include "jx-af/jx/JXUrgentTask.h"
#include "jx-af/jx/JXMenuManager.h"
#include "jx-af/jx/JXHelpManager.h"
#include "jx-af/jx/JXDocumentManager.h"
#include "jx-af/jx/JXSharedPrefsManager.h"
#include "jx-af/jx/JXMDIServer.h"
#include "jx-af/jx/JXAssert.h"
#include "jx-af/jx/jXEventUtil.h"
#include "jx-af/jx/jXGlobals.h"

#include <jx-af/jcore/JThisProcess.h>
#include <ace/Reactor.h>
#include <ace/Service_Config.h>
#include <sys/time.h>

#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jDirUtil.h>
#include <stdlib.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kDisplayOptionName         = "-display";
static const JUtf8Byte* kXDebugOptionName          = "--xdebug";
static const JUtf8Byte* kFTCHorizDebugOptionName   = "--debug-ftc-horiz";
static const JUtf8Byte* kFTCVertDebugOptionName    = "--debug-ftc-vert";
static const JUtf8Byte* kFTCDebugNoopOptionName    = "--debug-ftc-noop";
static const JUtf8Byte* kFTCDebugOverlapOptionName = "--debug-ftc-overlap";
static const JUtf8Byte* kDebugUtf8OptionName       = "--debug-utf8";
static const JUtf8Byte* kPseudotranslateOptionName = "--pseudotranslate";

const time_t kTimerStart = 100000000;	// seconds before rollover
const Time kMaxSleepTime = 50;			// 0.05 seconds (in milliseconds)

const JSize kWaitForChildCount = 10;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXApplication::JXApplication
	(
	int*				argc,
	char*				argv[],
	const JUtf8Byte*	appSignature,
	const JUtf8Byte**	defaultStringData
	)
	:
	JXDirector(nullptr),
	itsIgnoreDisplayDeletedFlag(false),
	itsIgnoreTaskDeletedFlag(false),
	itsRunningUrgentTasks(nullptr),
	itsSignature(appSignature),
	itsRestartCmd(argv[0])
{
	std::cout << std::boolalpha;	// since it will only be used for debugging
	std::cerr << std::boolalpha;

	itsTaskMutex = jnew std::recursive_mutex;
	assert( itsTaskMutex != nullptr );

	JString displayName;
	ParseBaseOptions(argc, argv, &displayName);

	// initialize object

	itsDisplayList = jnew JPtrArray<JXDisplay>(JPtrArrayT::kDeleteAll);
	assert( itsDisplayList != nullptr );

	itsCurrDisplayIndex = 1;

	itsIdleTaskStack = jnew IdleTaskStack(JPtrArrayT::kDeleteAll);
	assert( itsIdleTaskStack != nullptr );

	itsIdleTasks = jnew JPtrArray<JXIdleTask>(JPtrArrayT::kDeleteAll);
	assert( itsIdleTasks != nullptr );

	itsCurrentTime         = 0;
	itsMaxSleepTime        = 0;
	itsLastIdleTime        = 0;
	itsLastIdleTaskTime    = 0;
	itsWaitForChildCounter = 0;

	itsUrgentTasks = jnew JPtrArray<JXUrgentTask>(JPtrArrayT::kDeleteAll);
	assert( itsUrgentTasks != nullptr );

	itsHasBlockingWindowFlag = false;
	itsHadBlockingWindowFlag = false;
	itsRequestQuitFlag       = false;

	// if no path info specified, assume it's on exec path

	if (JIsRelativePath(itsRestartCmd) &&
		itsRestartCmd.Contains(ACE_DIRECTORY_SEPARATOR_STR))
	{
		const JString pwd = JGetCurrentDirectory();
		itsRestartCmd     = JCombinePathAndName(pwd, itsRestartCmd);
	}

	// initialize global objects

	JXCreateGlobals(this, appSignature, defaultStringData);
	ListenTo(JThisProcess::Instance());		// for SIGTERM

	// create display -- requires JXGetApplication() to work

	JXDisplay* display;
	if (!JXDisplay::Create(displayName, &display))
	{
		std::cerr << argv[0];
		if (displayName.IsEmpty())
		{
			std::cerr << ": Can't open display '" << XDisplayName(nullptr) << '\'';
		}
		else
		{
			std::cerr << ": Can't open display '" << displayName << '\'';
		}
		std::cerr << std::endl;
		JThisProcess::Exit(1);
	}

	if (JXGetSharedPrefsManager()->WasNew() && display->IsMacOS())
	{
		JXMenu::SetDisplayStyle(JXMenu::kMacintoshStyle);
	}

	// start the timer

#ifndef WIN32

	itimerval timerInfo;
	timerInfo.it_interval.tv_sec  = kTimerStart;
	timerInfo.it_interval.tv_usec = 0;
	timerInfo.it_value.tv_sec     = kTimerStart;
	timerInfo.it_value.tv_usec    = 0;
	if (setitimer(ITIMER_REAL, &timerInfo, nullptr) != 0)
	{
		std::cout << "setitimer failed: " << errno << std::endl;
		abort();
	}

#endif

	// idle task to quit if add directors deactivated

	auto* task = jnew JXQuitIfAllDeactTask;
	assert( task != nullptr );
	task->Start();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXApplication::~JXApplication()
{
	JXCloseDirectors();

	itsIgnoreDisplayDeletedFlag = true;

	jdelete itsDisplayList;
	JXDeleteGlobals1();

	itsIgnoreTaskDeletedFlag = true;

	jdelete itsIdleTaskStack;
	jdelete itsIdleTasks;
	jdelete itsUrgentTasks;
	jdelete itsTaskMutex;

	JXDeleteGlobals2();
}

/******************************************************************************
 Suspend (virtual)

 ******************************************************************************/

void
JXApplication::Suspend()
{
	for (auto* d : *itsDisplayList)
	{
		d->GetMenuManager()->CloseCurrentMenus();
	}

	JXDirector::Suspend();
	JXSuspendPersistentWindows();
}

/******************************************************************************
 Resume (virtual)

 ******************************************************************************/

void
JXApplication::Resume()
{
	JXDirector::Resume();
	JXResumePersistentWindows();
}

/******************************************************************************
 OpenDisplay

 ******************************************************************************/

bool
JXApplication::OpenDisplay
	(
	const JString&	displayName,
	JIndex*			displayIndex
	)
{
	assert( !displayName.IsEmpty() );

	JXDisplay* display;
	if (JXDisplay::Create(displayName, &display))
	{
		// DisplayOpened() appends new JXDisplay* to our list
		*displayIndex = itsDisplayList->GetElementCount();
		return true;
	}
	else
	{
		const JUtf8Byte* map[] =
		{
			"name", displayName.GetBytes()
		};
		JGetUserNotification()->ReportError(JGetString("DisplayConnectError", map, sizeof(map)));

		*displayIndex = 0;
		return false;
	}
}

/******************************************************************************
 DisplayOpened

	OpenDisplay() assumes that Append() is called.

 ******************************************************************************/

void
JXApplication::DisplayOpened
	(
	JXDisplay* display
	)
{
	itsDisplayList->Append(display);
	(JXGetAssertHandler())->DisplayOpened(display);
}

/******************************************************************************
 DisplayClosed

 ******************************************************************************/

void
JXApplication::DisplayClosed
	(
	JXDisplay* display
	)
{
	if (!itsIgnoreDisplayDeletedFlag)
	{
		itsDisplayList->Remove(display);
		(JXGetAssertHandler())->DisplayClosed(display);
	}
}

/******************************************************************************
 SetCurrentDisplay

 ******************************************************************************/

void
JXApplication::SetCurrentDisplay
	(
	const JIndex index
	)
{
	assert( itsDisplayList->IndexValid(index) );
	itsCurrDisplayIndex = index;
}

void
JXApplication::SetCurrentDisplay
	(
	JXDisplay* display
	)
{
	JIndex index;
	if (itsDisplayList->Find(display, &index))
	{
		itsCurrDisplayIndex = index;
	}
}

/******************************************************************************
 FindDisplay

	Returns true if the given display exists.

 ******************************************************************************/

bool
JXApplication::FindDisplay
	(
	const Display*	xDisplay,
	JXDisplay**		display
	)
{
	for (auto* d : *itsDisplayList)
	{
		if (d->GetXDisplay() == xDisplay)
		{
			*display = d;
			return true;
		}
	}

	*display = nullptr;
	return false;
}

/******************************************************************************
 DisplayBusyCursor

	Displays the watch cursor in all windows on all displays.

 ******************************************************************************/

void
JXApplication::DisplayBusyCursor()
{
	for (auto* d : *itsDisplayList)
	{
		d->DisplayCursorInAllWindows(kJXBusyCursor);
	}
}

/******************************************************************************
 DisplayInactiveCursor

	Displays the X cursor in all windows on all displays.

 ******************************************************************************/

void
JXApplication::DisplayInactiveCursor()
{
	for (auto* d : *itsDisplayList)
	{
		d->DisplayCursorInAllWindows(kJXInactiveCursor);
	}
}

/******************************************************************************
 HideAllWindows

 ******************************************************************************/

void
JXApplication::HideAllWindows()
{
	for (auto* d : *itsDisplayList)
	{
		d->HideAllWindows();
	}
}

/******************************************************************************
 Run

	When this function returns, JX and JCore have been shut down and
	the application object has been deleted.

 ******************************************************************************/

void
JXApplication::Run()
{
	if (setjmp(JThisProcess::GetSigintJumpBuffer()) != 0)
	{
		// assert fired is the closest to what we want - we are locked up, so user hit ctrl-c
		Abort(JXDocumentManager::kAssertFired, false);
		// does not return
	}

	while (true)
	{
		HandleOneEvent();

		if (itsRequestQuitFlag || !HasSubdirectors())
		{
			itsRequestQuitFlag = true;
			if (Close())
			{
				break;		// we have been deleted
			}
			else
			{
				itsRequestQuitFlag = false;
			}
		}
	}
}

/******************************************************************************
 Quit (virtual)

	Derived classes can override this to perform additional operations
	or to give the user a warning.  They *must* call the inherited version
	in order for it to take effect.

 ******************************************************************************/

void
JXApplication::Quit()
{
	itsRequestQuitFlag = true;
}

/******************************************************************************
 Close (virtual protected)

	This will fail unless Quit() has been called.

 ******************************************************************************/

bool
JXApplication::Close()
{
	assert( itsRequestQuitFlag );

	for (auto* d : *itsDisplayList)
	{
		d->GetMenuManager()->CloseCurrentMenus();
	}

	return JXDirector::Close();		// deletes us if successful
}

/******************************************************************************
 UpdateCurrentTime (protected)

	Calculate our current time from the current timer value.

 ******************************************************************************/

void
JXApplication::UpdateCurrentTime()
{
#ifdef WIN32

	itsCurrentTime = GetTickCount();

#else

	itimerval timerInfo;
	getitimer(ITIMER_REAL, &timerInfo);
	itsCurrentTime = JRound(1000 * (kTimerStart -
		 (timerInfo.it_value.tv_sec + timerInfo.it_value.tv_usec/1e6)));

#endif
}

/******************************************************************************
 HandleOneEvent (private)

	We process one event for each display.  If there are none, we idle.

	To process custom events, override HandleCustomEvent().

 ******************************************************************************/

void
JXApplication::HandleOneEvent()
{
	itsHadBlockingWindowFlag = false;

	UpdateCurrentTime();
	const bool allowSleep = HandleCustomEvent();

	UpdateCurrentTime();
	bool hasEvents = false;

	JPtrArrayIterator<JXDisplay> iter(itsDisplayList);
	JXDisplay* display;
	JIndex displayIndex = 0;
	while (iter.Next(&display))
	{
		displayIndex++;
		itsCurrDisplayIndex = displayIndex;		// itsCurrDisplayIndex might change during event
		if (XPending(*display) != 0)
		{
			hasEvents = true;

			// get the next event

			XEvent xEvent;
			XNextEvent(*display, &xEvent);

			if (XFilterEvent(&xEvent, None))	// XIM
			{
				continue;
			}

			if (xEvent.type != MotionNotify)
			{
				itsLastIdleTime = itsCurrentTime;
			}

			// dispatch the event

			display->HandleEvent(xEvent, itsCurrentTime);
		}
		else
		{
			display->Idle(itsCurrentTime);
		}
	}

	PopAllIdleTaskStack();
	PerformTasks(hasEvents, allowSleep);
}

/******************************************************************************
 HandleCustomEvent (virtual protected)

	This function exists to support real-time applications which need
	to modify the main event loop.  It is called every time through the
	main event loop.  (HandleCustomEventWhileBlocking() is called when a
	blocking dialog window is active.)

	The derived class is not required to return after handling a single
	event, but if it waits too long before returning, then the graphical
	interface will react sluggishly.

	The return value controls whether or not the main event loop idles
	by calling JWait().  The default is to return true to avoid hogging
	CPU time.  If the derived class handles the sleeping via some other
	system call, then it should return false.  Otherwise, it might return
	true if there were no events and false if there were.

 ******************************************************************************/

bool
JXApplication::HandleCustomEvent()
{
	return true;
}

/******************************************************************************
 HandleOneEventForWindow (private)

	We process one event for one window on one display.  We process Expose
	events for everybody.  We toss mouse and keyboard events for all other
	windows.

	In order to allow menus inside the window, we always pass all events
	to the current mouse and keyboard grabber windows.

	Returns true if we processed an event for the specified window.

	This enables the equivalent of await in JavaScript, which is the only
	known way to avoid callback-hell.

 ******************************************************************************/

const JSize kEventWindowCount = 3;

struct DiscardEventInfo
{
	JXDisplay*	display;
	Window*		eventWindow;	// pass to GetNextWindowEvent()

	DiscardEventInfo(JXDisplay* d, Window* w)
		:
		display(d), eventWindow(w)
	{ };
};

bool
JXApplication::HandleOneEventForWindow
	(
	JXWindow*	window,
	const bool	origAllowSleep
	)
{
	const bool origHadBlockingWindowFlag = itsHadBlockingWindowFlag;

	itsHasBlockingWindowFlag = true;
	itsHadBlockingWindowFlag = false;		// req'd by JXWindow

{
	std::lock_guard lock(*itsTaskMutex);
	if (itsIdleTaskStack->IsEmpty())
	{
		PushIdleTaskStack();
	}
}
	UpdateCurrentTime();
	const bool allowSleep =
		origAllowSleep && HandleCustomEventWhileBlocking();

	UpdateCurrentTime();
	bool windowHasEvents = false;

	const JXDisplay* uiDisplay = window->GetDisplay();

	Window eventWindow [ kEventWindowCount ];
	eventWindow[0] = window->GetXWindow();

	JPtrArrayIterator<JXDisplay> iter(itsDisplayList);
	JXDisplay* display;
	JIndex displayIndex = 0;
	while (iter.Next(&display))
	{
		JXMenuManager* menuMgr = display->GetMenuManager();
		if (!origHadBlockingWindowFlag)
		{
			menuMgr->CloseCurrentMenus();
		}

		JXWindow* mouseContainer;

		displayIndex++;
		itsCurrDisplayIndex = displayIndex;		// itsCurrDisplayIndex might change during event
		if (XPending(*display) != 0)
		{
			// get mouse and keyboard grabbers -- for menus inside blocking window

			eventWindow[1] = eventWindow[2] = None;

			JXWindow* grabber;
			if (display == uiDisplay && display->GetMouseGrabber(&grabber) &&
				menuMgr->IsMenuForWindow(grabber, window))
			{
				eventWindow[1] = grabber->GetXWindow();
			}
			if (display == uiDisplay && display->GetKeyboardGrabber(&grabber) &&
				menuMgr->IsMenuForWindow(grabber, window))
			{
				eventWindow[2] = grabber->GetXWindow();
			}

			// process one event

			XEvent xEvent;
			if (display == uiDisplay &&
				XCheckIfEvent(*display, &xEvent, GetNextWindowEvent,
							  reinterpret_cast<char*>(eventWindow)))
			{
				if (XFilterEvent(&xEvent, None))	// XIM
				{
					continue;
				}

				windowHasEvents = true;
				if (xEvent.type != MotionNotify)
				{
					itsLastIdleTime = itsCurrentTime;
				}
				display->HandleEvent(xEvent, itsCurrentTime);
			}
			else if (XCheckIfEvent(*display, &xEvent, GetNextBkgdEvent, nullptr))
			{
				if (XFilterEvent(&xEvent, None))	// XIM
				{
					continue;
				}
				display->HandleEvent(xEvent, itsCurrentTime);
			}
			else if (display == uiDisplay &&
					 display->GetMouseContainer(&mouseContainer) &&
					 mouseContainer == window)
			{
				display->Idle(itsCurrentTime);
			}
			else
			{
				display->Update();
			}

			// discard mouse and keyboard events

			DiscardEventInfo discardInfo(display, nullptr);
			if (display == uiDisplay)
			{
				discardInfo.eventWindow = eventWindow;
			}
			while (XCheckIfEvent(*display, &xEvent, DiscardNextEvent,
								 reinterpret_cast<char*>(&discardInfo)))
			{ /* discard events */ }
		}
		else if (display == uiDisplay &&
				 display->GetMouseContainer(&mouseContainer) &&
				 mouseContainer == window)
		{
			display->Idle(itsCurrentTime);
		}
		else
		{
			display->Update();
		}
	}

	PerformTasks(windowHasEvents, allowSleep);

	itsHasBlockingWindowFlag = false;
	itsHadBlockingWindowFlag = true;

	return windowHasEvents;
}

// static private

Bool
JXApplication::GetNextWindowEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	const Window currWindow = (event->xany).window;

	auto* eventWindow = reinterpret_cast<Window*>(arg);
	for (JUnsignedOffset i=0; i<kEventWindowCount; i++)
	{
		if (currWindow == eventWindow[i])
		{
			return True;
		}
	}

	return False;
}

Bool
JXApplication::GetNextBkgdEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	// Expose:           redraw window
	// ConfigureNotify:  redraw window correctly when resized
	// Map/UnmapNotify:  redraw window when deiconified
	// SelectionRequest: paste to other program
	// SelectionClear:   yield ownership so paste works correctly in CSF

	if (event->type == Expose           ||
		event->type == ConfigureNotify  ||
		event->type == MapNotify        ||
		event->type == UnmapNotify      ||
		event->type == SelectionRequest ||
		event->type == SelectionClear)
	{
		return True;
	}
	else
	{
		return False;
	}
}

Bool
JXApplication::DiscardNextEvent
	(
	Display*	display,
	XEvent*		event,
	char*		arg
	)
{
	auto* info = reinterpret_cast<DiscardEventInfo*>(arg);

	if (info->eventWindow != nullptr &&
		GetNextWindowEvent(display, event, reinterpret_cast<char*>(info->eventWindow)))
	{
		return False;
	}
	else if (event->type == KeyPress || event->type == KeyRelease ||
			 event->type == ButtonPress || event->type == ButtonRelease ||
			 event->type == MotionNotify ||
			 JXWindow::IsDeleteWindowMessage(info->display, *event))
	{
		return True;
	}
	else
	{
		return False;
	}
}

/******************************************************************************
 PerformTasks (private)

	Perform idle tasks when we don't receive any events and during long
	intervals of "mouse moved".

 ******************************************************************************/

void
JXApplication::PerformTasks
	(
	const bool hadEvents,
	const bool allowSleep
	)
{
	if (!hadEvents)
	{
		PerformIdleTasks();
		itsLastIdleTime = itsCurrentTime;
		PerformUrgentTasks();
		if (allowSleep)
		{
			JWait(itsMaxSleepTime / 1000.0);
		}
	}
	else if (hadEvents &&
			 itsCurrentTime - itsLastIdleTime > itsMaxSleepTime)
	{
		PerformIdleTasks();
		itsLastIdleTime = itsCurrentTime;
		PerformUrgentTasks();
	}
	else
	{
		PerformUrgentTasks();
	}
}

/******************************************************************************
 HandleCustomEventWhileBlocking (virtual protected)

	This function exists to support real-time applications which need
	to modify the subsidiary event loop that runs while a blocking dialog is
	active.  It is called every time through the event loop.

	Derived classes may just call HandleCustomEvent(), but they must first
	be sure that it is safe to do so.  The subsidiary event loop is only
	called when the main event loop is blocked.  This means that there may
	be an arbitrary amount of state stored on the execution stack and
	the affected objects may not be re-entrant.

	The derived class is not required to return after handling a single
	event, but if it waits too long before returning, then the graphical
	interface will react sluggishly.  This is especially important if the
	blocking window is a progress display, because the more often one
	returns, the sooner the window will disappear, thereby returning to
	the main event loop.

	The return value controls whether or not the main event loop idles
	by calling JWait().  The default is to return true to avoid hogging
	CPU time.  If the derived class handles the sleeping via some other
	system call, then it should return false.  Otherwise, it might return
	true if there were no events and false if there were.

 ******************************************************************************/

bool
JXApplication::HandleCustomEventWhileBlocking()
{
	return true;
}

/******************************************************************************
 InstallIdleTask

	We prepend the task so it will be performed next time if PerformIdleTasks()
	is executing.  This ensures that we will eventually reach the end
	of the task list.

 ******************************************************************************/

void
JXApplication::InstallIdleTask
	(
	JXIdleTask* newTask
	)
{
	std::lock_guard lock(*itsTaskMutex);

	if (!itsIdleTasks->Includes(newTask))
	{
		itsIdleTasks->Prepend(newTask);

		// Make sure it isn't stored anywhere else, so PopIdleTaskTask()
		// doesn't have to worry about duplicates when merging lists.

		for (auto* list : *itsIdleTaskStack)
		{
			list->Remove(newTask);
		}
	}
}

/******************************************************************************
 RemoveIdleTask

 ******************************************************************************/

void
JXApplication::RemoveIdleTask
	(
	JXIdleTask* task
	)
{
	if (!itsIgnoreTaskDeletedFlag)
	{
		std::lock_guard lock(*itsTaskMutex);

		itsIdleTasks->Remove(task);

		for (auto* list : *itsIdleTaskStack)
		{
			list->Remove(task);
		}
	}
}

/******************************************************************************
 PushIdleTaskStack (private)

 ******************************************************************************/

void
JXApplication::PushIdleTaskStack()
{
	std::lock_guard lock(*itsTaskMutex);

	itsIdleTaskStack->Append(itsIdleTasks);

	itsIdleTasks = jnew JPtrArray<JXIdleTask>(JPtrArrayT::kDeleteAll);
	assert( itsIdleTasks != nullptr );
}

/******************************************************************************
 PopIdleTaskStack (private)

	Since every object is required to delete its idle tasks, any
	remaining tasks should stay active.  (e.g. tasks installed by
	documents while a progress display is active)

 ******************************************************************************/

void
JXApplication::PopIdleTaskStack()
{
	std::lock_guard lock(*itsTaskMutex);

	if (!itsIdleTaskStack->IsEmpty())
	{
		JPtrArray<JXIdleTask>* list = itsIdleTasks;
		itsIdleTasks                = itsIdleTaskStack->GetLastElement();
		itsIdleTaskStack->RemoveElement(itsIdleTaskStack->GetElementCount());

		itsIdleTasks->CopyPointers(*list, JPtrArrayT::kDeleteAll, true);
		list->SetCleanUpAction(JPtrArrayT::kForgetAll);
		jdelete list;
	}
}

/******************************************************************************
 PopAllIdleTaskStack (private)

	This could be optimized, but the test for IsEmpty() ensures that
	it almost never has to do any work.

 ******************************************************************************/

void
JXApplication::PopAllIdleTaskStack()
{
	std::lock_guard lock(*itsTaskMutex);

	while (!itsIdleTaskStack->IsEmpty())
	{
		PopIdleTaskStack();
	}
}

/******************************************************************************
 PerformIdleTasks (private)

 ******************************************************************************/

void
JXApplication::PerformIdleTasks()
{
	itsMaxSleepTime = kMaxSleepTime;

	JPtrArray<JXIdleTask>* taskList = nullptr;
{
	std::lock_guard lock(*itsTaskMutex);

	if (!itsIdleTasks->IsEmpty())		// avoid constructing iterator
	{
		taskList = jnew JPtrArray<JXIdleTask>(*itsIdleTasks, JPtrArrayT::kForgetAll);
		assert( taskList != nullptr);
	}
}
	if (taskList != nullptr)
	{
		const Time deltaTime = itsCurrentTime - itsLastIdleTaskTime;

		JListIterator<JXIdleTask*>* iter = taskList->NewIterator();
		JXIdleTask* task;
		while (iter->Next(&task))
		{
			Time maxSleepTime = itsMaxSleepTime;
			task->Perform(deltaTime, &maxSleepTime);
			if (maxSleepTime < itsMaxSleepTime)
			{
				itsMaxSleepTime = maxSleepTime;
			}
		}

		jdelete iter;

		jdelete taskList;
		taskList = nullptr;
	}

	if (!itsHasBlockingWindowFlag)
	{
		// let sockets broadcast

		CheckACEReactor();

		// let processes broadcast -- not necessary to check each time

		itsWaitForChildCounter++;
		if (itsWaitForChildCounter >= kWaitForChildCount)
		{
			JProcess::CheckForFinishedChild(false);
			itsWaitForChildCounter = 0;
		}
	}

	JXMDIServer* mdiServer = nullptr;
	if (JXGetMDIServer(&mdiServer))
	{
		mdiServer->CheckForConnections();
	}

	// save time for next call

	itsLastIdleTaskTime = itsCurrentTime;
}

/******************************************************************************
 CheckACEReactor (static)

	Bumps the ACE reactor to check sockets, signals, etc.

	It is not generally a good idea to call this if there is a blocking
	window open because it may invoke arbitrary amounts of code, which is
	dangerous since JX is not re-entrant.

 ******************************************************************************/

void
JXApplication::CheckACEReactor()
{
	JThisProcess::CheckACEReactor();
}

/******************************************************************************
 InstallUrgentTask

	We insert the task in front of the execution iterator so it will be
	performed next time, if PerformUrgentTasks is executing.  This ensures
	that we will eventually reach the end of the task list.

 ******************************************************************************/

void
JXApplication::InstallUrgentTask
	(
	JXUrgentTask* newTask
	)
{
	std::lock_guard lock(*itsTaskMutex);

	if (!itsUrgentTasks->Includes(newTask))
	{
		itsUrgentTasks->Append(newTask);
	}
}

/******************************************************************************
 RemoveUrgentTask

 ******************************************************************************/

void
JXApplication::RemoveUrgentTask
	(
	JXUrgentTask* task
	)
{
	if (!itsIgnoreTaskDeletedFlag)
	{
		std::lock_guard lock(*itsTaskMutex);

		itsUrgentTasks->Remove(task);

		if (itsRunningUrgentTasks != nullptr)
		{
			itsRunningUrgentTasks->Remove(task);
		}
	}
}

/******************************************************************************
 PerformUrgentTasks (private)

 ******************************************************************************/

void
JXApplication::PerformUrgentTasks()
{
	JPtrArray<JXUrgentTask>* taskList = nullptr;
{
	std::lock_guard lock(*itsTaskMutex);

	if (!itsUrgentTasks->IsEmpty())
	{
		// clear out itsUrgentTasks so new ones can safely be added

		taskList = jnew JPtrArray<JXUrgentTask>(*itsUrgentTasks, JPtrArrayT::kDeleteAll);
		assert( taskList != nullptr);

		itsUrgentTasks->RemoveAll();
	}
}
	if (taskList != nullptr)
	{
		itsRunningUrgentTasks = taskList;

		// perform each task - use iterator because task might be deleted

		JListIterator<JXUrgentTask*>* iter = taskList->NewIterator();
		JXUrgentTask* task;
		while (iter->Next(&task))
		{
			task->Perform();
		}

		itsRunningUrgentTasks = nullptr;
		jdelete iter;

		jdelete taskList;
		taskList = nullptr;
	}

	JXDisplay::CheckForXErrors();

	// We check in this order so CheckForSignals() can broadcast even
	// if the app is suspended.

	if (!itsHasBlockingWindowFlag && JThisProcess::CheckForSignals() &&
		!IsSuspended())
	{
		Quit();
	}
}

/******************************************************************************
 ParseBaseOptions (static private)

	This is static because it is called before any other code in the
	constructor.

 ******************************************************************************/

void
JXApplication::ParseBaseOptions
	(
	int*		argc,
	char*		argv[],
	JString*	displayName
	)
{
	displayName->Clear();

	bool ftcNoop = false, ftcOverlap = false;

	for (long i=1; i<*argc; i++)
	{
		if (strcmp(argv[i], kDisplayOptionName) == 0)
		{
			i++;
			if (i >= *argc || argv[i][0] == '-')
			{
				std::cerr << argv[0] << ": Invalid display option" << std::endl;
				JThisProcess::Exit(1);
			}
			*displayName = argv[i];
			RemoveCmdLineOption(argc, argv, i-1, 2);
			i -= 2;
		}
		else if (strcmp(argv[i], kXDebugOptionName) == 0)
		{
			_Xdebug = True;
			RemoveCmdLineOption(argc, argv, i, 1);
			i--;
		}
		else if (strcmp(argv[i], kFTCHorizDebugOptionName) == 0)
		{
			JXContainer::DebugExpandToFitContent(true);
			RemoveCmdLineOption(argc, argv, i, 1);
			i--;
		}
		else if (strcmp(argv[i], kFTCVertDebugOptionName) == 0)
		{
			JXContainer::DebugExpandToFitContent(false);
			RemoveCmdLineOption(argc, argv, i, 1);
			i--;
		}
		else if (strcmp(argv[i], kFTCDebugNoopOptionName) == 0)
		{
			ftcNoop = true;
			RemoveCmdLineOption(argc, argv, i, 1);
			i--;
		}
		else if (strcmp(argv[i], kFTCDebugOverlapOptionName) == 0)
		{
			ftcOverlap = true;
			RemoveCmdLineOption(argc, argv, i, 1);
			i--;
		}
		else if (strcmp(argv[i], kDebugUtf8OptionName) == 0)
		{
			JUtf8Character::SetIgnoreBadUtf8(false);
			RemoveCmdLineOption(argc, argv, i, 1);
			i--;
		}
		else if (strcmp(argv[i], kPseudotranslateOptionName) == 0)
		{
			JStringManager::EnablePseudotranslation();
			RemoveCmdLineOption(argc, argv, i, 1);
			i--;
		}
	}

	JXContainer::DebugExpandToFitContentExtras(ftcNoop, ftcOverlap);
}

/******************************************************************************
 StripBaseOptions (static)

 ******************************************************************************/

void
JXApplication::StripBaseOptions
	(
	JPtrArray<JString>* argList
	)
{
	JSize count = argList->GetElementCount();
	for (JIndex i=2; i<=count; i++)
	{
		JString* arg = argList->GetElement(i);
		if (*arg == kXDebugOptionName        ||
			*arg == kFTCHorizDebugOptionName ||
			*arg == kFTCVertDebugOptionName  ||
			*arg == kPseudotranslateOptionName)
		{
			argList->DeleteElement(i);
			count--;
			i--;
		}
		else if (*arg == kDisplayOptionName)
		{
			argList->DeleteElement(i);
			argList->DeleteElement(i);
			count -= 2;
			i--;
		}
	}
}

/******************************************************************************
 RemoveCmdLineOption (static)

 ******************************************************************************/

void
JXApplication::RemoveCmdLineOption
	(
	int*				argc,
	char*				argv[],
	const unsigned long	offset,
	const unsigned long	removeCount
	)
{
	const long firstKeep = offset + removeCount;
	assert( firstKeep <= *argc );

	for (long i=firstKeep; i<*argc; i++)
	{
		argv[i - removeCount] = argv[i];
	}

	*argc -= removeCount;
}

/******************************************************************************
 ReceiveWithFeedback (virtual protected)

	SIGTERM can mean that the system is shutting down.

 ******************************************************************************/

void
JXApplication::ReceiveWithFeedback
	(
	JBroadcaster*	sender,
	Message*		message
	)
{
	if (sender == JThisProcess::Instance() && message->Is(JThisProcess::kTerminate))
	{
		Abort(JXDocumentManager::kKillSignal, false);
	}
	else
	{
		JXDirector::ReceiveWithFeedback(sender, message);
	}
}

/******************************************************************************
 JXIOErrorHandler (static)

	Calls JXApplication::Abort().

 ******************************************************************************/

int
JXApplication::JXIOErrorHandler
	(
	Display* xDisplay
	)
{
	Abort(JXDocumentManager::kServerDead, false);
}

/******************************************************************************
 Abort (static)

	When something explodes, we get one last chance to clean things up before
	we have to exit.  We trigger a safety save and then give derived classes
	a chance to save other things, like preferences.  If something explodes
	again, we give up and die immediately.

	This function does not return.

 ******************************************************************************/

static bool abortCalled = false;

void
JXApplication::Abort
	(
	const JXDocumentManager::SafetySaveReason	reason,
	const bool								dumpCore
	)
{
	if (!abortCalled)
	{
		abortCalled = true;

		JXDocumentManager* docMgr = nullptr;
		if (JXGetDocumentManager(&docMgr))
		{
			docMgr->SafetySave(reason);
		}

		JXApplication* app;
		if (JXGetApplication(&app))
		{
			app->CleanUpBeforeSuddenDeath(reason);
		}
	}
	else
	{
		fprintf(stderr, "\nError inside XIO fatal error handler!\n\n");
	}

	if (dumpCore)
	{
		JThisProcess::Abort();
	}
	else
	{
		JThisProcess::Exit(1);
	}
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
JXApplication::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
}
