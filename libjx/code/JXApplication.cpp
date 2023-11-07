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

#include "JXApplication.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXFunctionTask.h"
#include "JXUrgentTask.h"
#include "JXMenuManager.h"
#include "JXHelpManager.h"
#include "JXDocumentManager.h"
#include "JXSharedPrefsManager.h"
#include "JXMDIServer.h"
#include "JXBoostPriorityScheduler.h"
#include "JXAssert.h"
#include "jXEventUtil.h"
#include "jXGlobals.h"

#include <jx-af/jcore/JThisProcess.h>
#include <ace/Reactor.h>
#include <ace/Service_Config.h>
#include <sys/time.h>

#include <jx-af/jcore/JTaskIterator.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jDirUtil.h>
#include <algorithm>
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

const Time kCheckQuitPeriod    = 30001;	// 30 seconds (milliseconds)
const JSize kWaitForChildCount = 10;

static thread_local bool theUIThreadFlag = false;

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
	itsCurrDisplayIndex(1),
	itsIgnoreDisplayDeletedFlag(false),
	itsIgnoreTaskDeletedFlag(false),
	itsCurrentTime(0),
	itsMaxSleepTime(0),
	itsLastIdleTime(0),
	itsLastIdleTaskTime(0),
	itsWaitForChildCounter(0),
	itsSignature(appSignature),
	itsRestartCmd(argv[0]),
	itsRequestQuitFlag(false),
	itsIsQuittingFlag(false)
{
	theUIThreadFlag = true;

	std::cout << std::boolalpha;	// since it will only be used for debugging
	std::cerr << std::boolalpha;

	itsTaskMutex = jnew std::recursive_mutex;
	assert( itsTaskMutex != nullptr );

	JString displayName;
	ParseBaseOptions(argc, argv, &displayName);

	// initialize object

	itsDisplayList = jnew JPtrArray<JXDisplay>(JPtrArrayT::kDeleteAll);
	assert( itsDisplayList != nullptr );

	itsIdleTasks = jnew JPtrArray<JXIdleTask>(JPtrArrayT::kDeleteAll);
	assert( itsIdleTasks != nullptr );

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

	// init fiber manager - required for JXWindow::AnalyzeWindowManager()

	boost::fibers::use_scheduling_algorithm<JXBoostPriorityScheduler>();

	JSetTaskScheduler(std::bind(JXApplication::StartFiber, std::placeholders::_1, kIdleTaskPriority));

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

	auto* task = jnew JXFunctionTask(kCheckQuitPeriod, [this]()
	{
		const JPtrArray<JXDirector>* list;
		if (!GetSubdirectors(&list) ||
			std::all_of(begin(*list), end(*list),
				[](JXDirector* dir) { return !dir->IsActive(); }))
		{
			Quit();
		}
	});
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

	jdelete itsIdleTasks;
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
		*displayIndex = itsDisplayList->GetItemCount();
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

		if (itsRequestQuitFlag)
		{
			itsRequestQuitFlag = false;
			itsIsQuittingFlag  = true;
			StartFiber([this]()
			{
				if (!CloseAllSubdirectors())
				{
					itsIsQuittingFlag = false;
				}
			});
		}
		else if (!HasSubdirectors())
		{
			itsIsQuittingFlag = true;
			if (Close())
			{
				break;	// we have been deleted
			}
			else
			{
				itsIsQuittingFlag = false;
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
	assert( itsIsQuittingFlag );

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

	Each event is processed in a separate fiber to allow blocking for the
	result of a dialog.

 ******************************************************************************/

void
JXApplication::HandleOneEvent()
{
	UpdateCurrentTime();

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

			const Time t = this->itsCurrentTime;
			StartFiber([display, xEvent, t]()
			{
				display->HandleEvent(xEvent, t);
			},
			kEventHandlerPriority);

			boost::this_fiber::yield();
		}
		else
		{
			display->Idle(itsCurrentTime);
		}
	}

	StartTasks(hasEvents);
}

/******************************************************************************
 StartTasks (private)

	Perform idle tasks when we don't receive any events and during long
	intervals of "mouse moved".

 ******************************************************************************/

void
JXApplication::StartTasks
	(
	const bool hadEvents
	)
{
	StartFiber(JXDisplay::CheckForXErrors, kUrgentTaskPriority);

	StartFiber([this]()
	{
		// We check in this order so CheckForSignals() can broadcast even
		// if the app is suspended.

		if (JThisProcess::CheckForSignals() && !IsSuspended())
		{
			Quit();
		}
	},
	kUrgentTaskPriority);

	boost::this_fiber::yield();

	if (!hadEvents)
	{
		StartIdleTasks();
		itsLastIdleTime = itsCurrentTime;
		JWait(itsMaxSleepTime / 1000.0);
	}
	else if (itsCurrentTime - itsLastIdleTime > itsMaxSleepTime)
	{
		StartIdleTasks();
		itsLastIdleTime = itsCurrentTime;
	}
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
	JXIdleTask* task
	)
{
	std::lock_guard lock(*itsTaskMutex);

	if (!itsIdleTasks->Includes(task))
	{
		itsIdleTasks->Prepend(task);
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
	}
}

/******************************************************************************
 StartIdleTasks (private)

 ******************************************************************************/

void
JXApplication::StartIdleTasks()
{
	itsMaxSleepTime = kMaxSleepTime;
	{
	std::lock_guard lock(*itsTaskMutex);

	if (!itsIdleTasks->IsEmpty())		// avoid constructing iterator
	{
		const Time deltaTime = itsCurrentTime - itsLastIdleTaskTime;

		JTaskIterator<JXIdleTask> iter(itsIdleTasks);
		JXIdleTask* task;
		while (iter.Next(&task))
		{
			Time maxSleepTime = itsMaxSleepTime;
			if (task->Ready(deltaTime, &maxSleepTime))
			{
				StartFiber([task, deltaTime]()
				{
					task->Perform(deltaTime);
				});
			}

			if (maxSleepTime < itsMaxSleepTime)
			{
				itsMaxSleepTime = maxSleepTime;
			}
			boost::this_fiber::yield();		// wait for task to finish - might delete other tasks
		}
	}
	}

	// save time for next call

	itsLastIdleTaskTime = itsCurrentTime;

	// let sockets broadcast

	StartFiber(JThisProcess::CheckACEReactor);

	// let processes broadcast -- not necessary to check each time

	itsWaitForChildCounter++;
	if (itsWaitForChildCounter >= kWaitForChildCount)
	{
		StartFiber([this]()
		{
			JProcess::CheckForFinishedChild(false);
			itsWaitForChildCounter = 0;
		});
	}

	JXMDIServer* mdiServer = nullptr;
	if (JXGetMDIServer(&mdiServer))
	{
		StartFiber([mdiServer]()
		{
			mdiServer->CheckForConnections();
		});
	}

	boost::this_fiber::yield();
}

/******************************************************************************
 InstallUrgentTask

	We immediately create a fiber, so it will run as soon as possible.

 ******************************************************************************/

void
JXApplication::InstallUrgentTask
	(
	JXUrgentTask* task
	)
{
	StartFiber([task]()
	{
		if (!task->Cancelled())
		{
			task->Perform();
		}
		jdelete task;
	},
	kUrgentTaskPriority);
}

/******************************************************************************
 StartFiber (static)

 ******************************************************************************/

void
JXApplication::StartFiber
	(
	const std::function<void()>&	f,
	const FiberPriority				priority
	)
{
	assert( theUIThreadFlag );
	assert( priority > kEventLoopPriority );

	auto fiber = boost::fibers::fiber(f);
	fiber.properties<JXBoostPriorityProps>().SetPriority(priority);
	fiber.detach();
}

/******************************************************************************
 IsWorkerFiber (static)

 ******************************************************************************/

bool
JXApplication::IsWorkerFiber()
{
	return boost::this_fiber::properties<JXBoostPriorityProps>().GetPriority() > kEventLoopPriority;
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
	JSize count = argList->GetItemCount();
	for (JIndex i=2; i<=count; i++)
	{
		JString* arg = argList->GetItem(i);
		if (*arg == kXDebugOptionName        ||
			*arg == kFTCHorizDebugOptionName ||
			*arg == kFTCVertDebugOptionName  ||
			*arg == kPseudotranslateOptionName)
		{
			argList->DeleteItem(i);
			count--;
			i--;
		}
		else if (*arg == kDisplayOptionName)
		{
			argList->DeleteItem(i);
			argList->DeleteItem(i);
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
	const bool									dumpCore
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
		fprintf(stderr, "\nError inside JXApplication::Abort!\n\n");
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
