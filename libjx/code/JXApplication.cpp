/******************************************************************************
 JXApplication.cpp

	Initializes global objects and manages the event loop.

	Urgent tasks are performed after the current event and are then deleted.

	Idle tasks are performed whenever the event loop is idle.  It is safe
	to delete a JXIdleTask object because it will automatically remove itself
	from the task list.

	BASE CLASS = JXDirector

	Copyright © 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXApplication.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXIdleTask.h>
#include <JXQuitIfAllDeactTask.h>
#include <JXUrgentTask.h>
#include <JXMenuManager.h>
#include <JXHelpManager.h>
#include <JXDocumentManager.h>
#include <JXMDIServer.h>
#include <JXAssert.h>
#include <jXEventUtil.h>
#include <jXGlobals.h>

#include <JThisProcess.h>
#include <j_prep_ace.h>
#include <ace/Reactor.h>
#include <ace/Service_Config.h>
#include <sys/time.h>

#include <JTaskIterator.h>
#include <jTime.h>
#include <jDirUtil.h>
#include <stdlib.h>
#include <stdio.h>
#include <jMissingProto.h>
#include <jAssert.h>

static const JCharacter* kDisplayOptionName = "-display";
static const JCharacter* kXDebugOptionName  = "--xdebug";

const time_t kTimerStart = J_TIME_T_MAX/1000U;	// milliseconds before rollover
const Time kMaxSleepTime = 50;					// 0.05 seconds (in milliseconds)

const JSize kWaitForChildCount = 10;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXApplication::JXApplication
	(
	int*				argc,
	char*				argv[],
	const JCharacter*	appSignature,
	const JCharacter**	defaultStringData
	)
	:
	JXDirector(NULL),
	itsIgnoreDisplayDeletedFlag(kJFalse),
	itsIgnoreTaskDeletedFlag(kJFalse),
	itsSignature(appSignature),
	itsRestartCmd(argv[0])
{
	// initialize object

	itsDisplayList = new JPtrArray<JXDisplay>(JPtrArrayT::kDeleteAll);
	assert( itsDisplayList != NULL );

	itsCurrDisplayIndex = 1;

	itsIdleTaskStack = new IdleTaskStack(JPtrArrayT::kDeleteAll);
	assert( itsIdleTaskStack != NULL );

	itsIdleTasks = new JPtrArray<JXIdleTask>(JPtrArrayT::kDeleteAll);
	assert( itsIdleTasks != NULL );

	itsCurrentTime         = 0;
	itsMaxSleepTime        = 0;
	itsLastIdleTime        = 0;
	itsLastIdleTaskTime    = 0;
	itsWaitForChildCounter = 0;

	itsUrgentTasks = new JPtrArray<JXUrgentTask>(JPtrArrayT::kDeleteAll);
	assert( itsUrgentTasks != NULL );

	itsHasBlockingWindowFlag = kJFalse;
	itsHadBlockingWindowFlag = kJFalse;
	itsRequestQuitFlag       = kJFalse;

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

	JString displayName;
	ParseBaseOptions(argc, argv, &displayName);

	JXDisplay* display;
	if (!JXDisplay::Create(displayName, &display))
		{
		cerr << argv[0];
		if (displayName.IsEmpty())
			{
			cerr << ": Can't open display '" << XDisplayName(NULL) << '\'';
			}
		else
			{
			cerr << ": Can't open display '" << displayName << '\'';
			}
		cerr << endl;
		JThisProcess::Exit(1);
		}

	// start the timer

#ifndef WIN32

	itimerval timerInfo;
	timerInfo.it_interval.tv_sec  = kTimerStart;
	timerInfo.it_interval.tv_usec = 0;
	timerInfo.it_value.tv_sec     = kTimerStart;
	timerInfo.it_value.tv_usec    = 0;
	setitimer(ITIMER_REAL, &timerInfo, NULL);

#endif

	// idle task to quit if add directors deactivated

	JXQuitIfAllDeactTask* task = new JXQuitIfAllDeactTask;
	assert( task != NULL );
	task->Start();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXApplication::~JXApplication()
{
	JXCloseDirectors();

	itsIgnoreDisplayDeletedFlag = kJTrue;

	delete itsDisplayList;
	JXDeleteGlobals1();

	itsIgnoreTaskDeletedFlag = kJTrue;

	delete itsIdleTaskStack;
	delete itsIdleTasks;
	delete itsUrgentTasks;

	JXDeleteGlobals2();
}

/******************************************************************************
 Suspend (virtual)

 ******************************************************************************/

void
JXApplication::Suspend()
{
	const JSize count = itsDisplayList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsDisplayList->NthElement(i)->GetMenuManager()->CloseCurrentMenus();
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

JBoolean
JXApplication::OpenDisplay
	(
	const JCharacter*	displayName,
	JIndex*				displayIndex
	)
{
	assert( displayName != NULL && displayName[0] != '\0' );

	JXDisplay* display;
	if (JXDisplay::Create(displayName, &display))
		{
		// DisplayOpened() appends new JXDisplay* to our list
		*displayIndex = itsDisplayList->GetElementCount();
		return kJTrue;
		}
	else
		{
		JString errorStr = "Unable to connect to ";
		errorStr += displayName;
		(JGetUserNotification())->ReportError(errorStr);

		*displayIndex = 0;
		return kJFalse;
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

	Returns kJTrue if the given display exists.

 ******************************************************************************/

JBoolean
JXApplication::FindDisplay
	(
	const Display*	xDisplay,
	JXDisplay**		display
	)
{
	const JSize count = itsDisplayList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDisplay* d = itsDisplayList->NthElement(i);
		if (d->GetXDisplay() == xDisplay)
			{
			*display = d;
			return kJTrue;
			}
		}

	*display = NULL;
	return kJFalse;
}

/******************************************************************************
 DisplayBusyCursor

	Displays the watch cursor in all windows on all displays.

 ******************************************************************************/

void
JXApplication::DisplayBusyCursor()
{
	const JSize count = itsDisplayList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsDisplayList->NthElement(i))->DisplayCursorInAllWindows(kJXBusyCursor);
		}
}

/******************************************************************************
 DisplayInactiveCursor

	Displays the X cursor in all windows on all displays.

 ******************************************************************************/

void
JXApplication::DisplayInactiveCursor()
{
	const JSize count = itsDisplayList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsDisplayList->NthElement(i))->DisplayCursorInAllWindows(kJXInactiveCursor);
		}
}

/******************************************************************************
 HideAllWindows

 ******************************************************************************/

void
JXApplication::HideAllWindows()
{
	const JSize count = itsDisplayList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsDisplayList->NthElement(i))->HideAllWindows();
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
	while (1)
		{
		HandleOneEvent();

		if (itsRequestQuitFlag || !HasSubdirectors())
			{
			itsRequestQuitFlag = kJTrue;
			if (Close())
				{
				break;		// we have been deleted
				}
			else
				{
				itsRequestQuitFlag = kJFalse;
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
	itsRequestQuitFlag = kJTrue;
}

/******************************************************************************
 Close (virtual protected)

	This will fail unless Quit() has been called.

 ******************************************************************************/

JBoolean
JXApplication::Close()
{
	assert( itsRequestQuitFlag );

	const JSize count = itsDisplayList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		JXDisplay* display = itsDisplayList->NthElement(i);
		(display->GetMenuManager())->CloseCurrentMenus();
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
	itsHadBlockingWindowFlag = kJFalse;

	UpdateCurrentTime();
	const JBoolean allowSleep = HandleCustomEvent();

	UpdateCurrentTime();
	JBoolean hasEvents = kJFalse;

	JPtrArrayIterator<JXDisplay> iter(itsDisplayList);
	JXDisplay* display;
	JIndex displayIndex = 0;
	while (iter.Next(&display))
		{
		displayIndex++;
		itsCurrDisplayIndex = displayIndex;		// itsCurrDisplayIndex might change during event
		if (XPending(*display) != 0)
			{
			hasEvents = kJTrue;

			// get the next event

			XEvent xEvent;
			XNextEvent(*display, &xEvent);

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
	by calling JWait().  The default is to return kJTrue to avoid hogging
	CPU time.  If the derived class handles the sleeping via some other
	system call, then it should return kJFalse.  Otherwise, it might return
	kJTrue if there were no events and kJFalse if there were.

 ******************************************************************************/

JBoolean
JXApplication::HandleCustomEvent()
{
	return kJTrue;
}

/******************************************************************************
 HandleOneEventForWindow (private)

	We process one event for one window on one display.  We process Expose
	events for everybody.  We toss mouse and keyboard events for all other
	windows.

	In order to allow menus inside the window, we always pass all events
	to the current mouse and keyboard grabber windows.

	Returns kJTrue if we processed an event for the specified window.

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

JBoolean
JXApplication::HandleOneEventForWindow
	(
	JXWindow*		window,
	const JBoolean	origAllowSleep
	)
{
	const JBoolean origHadBlockingWindowFlag = itsHadBlockingWindowFlag;

	itsHasBlockingWindowFlag = kJTrue;
	itsHadBlockingWindowFlag = kJFalse;		// req'd by JXWindow

	if (itsIdleTaskStack->IsEmpty())
		{
		PushIdleTaskStack();
		}

	UpdateCurrentTime();
	const JBoolean allowSleep =
		JI2B(origAllowSleep && HandleCustomEventWhileBlocking());

	UpdateCurrentTime();
	JBoolean windowHasEvents = kJFalse;

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
				windowHasEvents = kJTrue;
				if (xEvent.type != MotionNotify)
					{
					itsLastIdleTime = itsCurrentTime;
					}
				display->HandleEvent(xEvent, itsCurrentTime);
				}
			else if (XCheckIfEvent(*display, &xEvent, GetNextBkgdEvent, NULL))
				{
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

			DiscardEventInfo discardInfo(display, NULL);
			if (display == uiDisplay)
				{
				discardInfo.eventWindow = eventWindow;
				}
			while (XCheckIfEvent(*display, &xEvent, DiscardNextEvent,
								 reinterpret_cast<char*>(&discardInfo)))
				{ };
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

	itsHasBlockingWindowFlag = kJFalse;
	itsHadBlockingWindowFlag = kJTrue;

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

	Window* eventWindow = reinterpret_cast<Window*>(arg);
	for (JIndex i=0; i<kEventWindowCount; i++)
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
	DiscardEventInfo* info = reinterpret_cast<DiscardEventInfo*>(arg);

	if (info->eventWindow != NULL &&
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
	const JBoolean hadEvents,
	const JBoolean allowSleep
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
	by calling JWait().  The default is to return kJTrue to avoid hogging
	CPU time.  If the derived class handles the sleeping via some other
	system call, then it should return kJFalse.  Otherwise, it might return
	kJTrue if there were no events and kJFalse if there were.

 ******************************************************************************/

JBoolean
JXApplication::HandleCustomEventWhileBlocking()
{
	return kJTrue;
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
	if (!itsIdleTasks->Includes(newTask))
		{
		itsIdleTasks->Prepend(newTask);

		// Make sure it isn't stored anywhere else, so PopIdleTaskTask()
		// doesn't have to worry about duplicates when merging lists.

		const JSize count = itsIdleTaskStack->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsIdleTaskStack->NthElement(i))->Remove(newTask);
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
		itsIdleTasks->Remove(task);

		const JSize count = itsIdleTaskStack->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsIdleTaskStack->NthElement(i))->Remove(task);
			}
		}
}

/******************************************************************************
 PushIdleTaskStack (private)

 ******************************************************************************/

void
JXApplication::PushIdleTaskStack()
{
	itsIdleTaskStack->Append(itsIdleTasks);

	itsIdleTasks = new JPtrArray<JXIdleTask>(JPtrArrayT::kDeleteAll);
	assert( itsIdleTasks != NULL );
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
	if (!itsIdleTaskStack->IsEmpty())
		{
		JPtrArray<JXIdleTask>* list = itsIdleTasks;
		itsIdleTasks                = itsIdleTaskStack->LastElement();
		itsIdleTaskStack->RemoveElement(itsIdleTaskStack->GetElementCount());

		itsIdleTasks->CopyPointers(*list, JPtrArrayT::kDeleteAll, kJTrue);
		list->SetCleanUpAction(JPtrArrayT::kForgetAll);
		delete list;
		}
}

/******************************************************************************
 PopAllIdleTaskStack (private)

	This could be optimized, but the test for IsEmpty() insures that
	it almost never has to do any work.

 ******************************************************************************/

void
JXApplication::PopAllIdleTaskStack()
{
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

	if (!itsIdleTasks->IsEmpty())		// avoid constructing iterator
		{
		JTaskIterator<JXIdleTask> iter(itsIdleTasks);
		JXIdleTask* task;
		const Time deltaTime = itsCurrentTime - itsLastIdleTaskTime;
		while (iter.Next(&task))
			{
			Time maxSleepTime = itsMaxSleepTime;
			task->Perform(deltaTime, &maxSleepTime);
			if (maxSleepTime < itsMaxSleepTime)
				{
				itsMaxSleepTime = maxSleepTime;
				}
			}
		}

	if (!itsHasBlockingWindowFlag)
		{
		// let sockets broadcast

		CheckACEReactor();

		// let processes broadcast -- not necessary to check each time

		itsWaitForChildCounter++;
		if (itsWaitForChildCounter >= kWaitForChildCount)
			{
			JProcess::CheckForFinishedChild(kJFalse);
			itsWaitForChildCounter = 0;
			}
		}

	JXMDIServer* mdiServer = NULL;
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
	performed next time, if PerformUrgentTasks is executing.  This insures
	that we will eventually reach the end of the task list.

 ******************************************************************************/

void
JXApplication::InstallUrgentTask
	(
	JXUrgentTask* newTask
	)
{
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
		itsUrgentTasks->Remove(task);
		}
}

/******************************************************************************
 PerformUrgentTasks (private)

 ******************************************************************************/

void
JXApplication::PerformUrgentTasks()
{
	if (!itsUrgentTasks->IsEmpty())
		{
		// clear out itsUrgentTasks so new ones can safely be added

		JPtrArray<JXUrgentTask> taskList(*itsUrgentTasks, JPtrArrayT::kDeleteAll);
		itsUrgentTasks->RemoveAll();

		// perform each task

		const JSize count = taskList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(taskList.NthElement(i))->Perform();
			}
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

	for (long i=1; i<*argc; i++)
		{
		if (strcmp(argv[i], kDisplayOptionName) == 0)
			{
			i++;
			if (i >= *argc || argv[i][0] == '-')
				{
				cerr << argv[0] << ": Invalid display option" << endl;
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
		}
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
		JString* arg = argList->NthElement(i);
		if (*arg == kXDebugOptionName)
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
		Abort(JXDocumentManager::kKillSignal, kJFalse);
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
	Abort(JXDocumentManager::kServerDead, kJFalse);
	return 0;
}

/******************************************************************************
 Abort (static)

	When something explodes, we get one last chance to clean things up before
	we have to exit.  We trigger a safety save and then give derived classes
	a chance to save other things, like preferences.  If something explodes
	again, we give up and die immediately.

	This function does not return.

 ******************************************************************************/

static JBoolean abortCalled = kJFalse;

void
JXApplication::Abort
	(
	const JXDocumentManager::SafetySaveReason	reason,
	const JBoolean								dumpCore
	)
{
	if (!abortCalled)
		{
		abortCalled = kJTrue;

		JXDocumentManager* docMgr = NULL;
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
