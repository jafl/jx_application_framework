/******************************************************************************
 jGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 1997 John Lindal.

 ******************************************************************************/

#include "jGlobals.h"
#include "jStringData.h"
#include "JTextUserNotification.h"
#include "JCreateTextPG.h"
#include "JThisProcess.h"
#include "JWebBrowser.h"
#include "jDirUtil.h"
#include "jFileUtil.h"
#include <stdlib.h>
#include <locale.h>
#include <ace/OS_NS_sys_socket.h>
#include "jAssert.h"

static JAssertBase*				theAssertHandler    = nullptr;

static JUserNotification*		theUserNotification = nullptr;
static JCreateProgressDisplay*	theCreatePG         = nullptr;

static JStringManager*			theStringManager    = nullptr;
static JWebBrowser*				theWebBrowser       = nullptr;

static std::function<void(const std::function<void()>&)>* theTaskScheduler = nullptr;

static thread_local bool theInteractiveThreadFlag = false;
static std::recursive_mutex theTempDirChangeMutex;

/******************************************************************************
 JInitCore

	Initializes all global objects and factories.  Calls atexit(JDeleteGlobals)
	to ensure that all objects are cleaned up when the program terminates.

	*** We take ownership of all the objects that are passed in.
		We make copies of the string data.

	Passing in nullptr for ah,un,cpg means that you want the default text
	implementation.

	Passing in nullptr for appSignature and defaultStringData means that you want
	to use the default U.S. string data.

	Passing in nullptr for gcfm,gcc means that you get nothing, so we will
	explode via assert() if these objects are requested.

	Passing in nullptr for the font names means that you get:
		defaultFontName   = "Helvetica"
		greekFontName     = "Symbol"
		monospaceFontName = "Courier"

 ******************************************************************************/

void
JInitCore
	(
	JAssertBase*			ah,

	const JUtf8Byte*		appSignature,
	const JUtf8Byte**		defaultStringData,

	JUserNotification*		un,
	JCreateProgressDisplay*	cpg
	)
{
	if (theStringManager != nullptr)
	{
		return;		// already initialized
	}

	// assert handler

	if (ah != nullptr)
	{
		theAssertHandler = ah;
	}
	// nullptr is valid in this case -- see JGetAssertHandler()

	// use native locale

	setlocale(LC_ALL, "");

	// socket library (mainly Windows)

	ACE_OS::socket_init();

	// signal handler

	JThisProcess::Initialize();

	// string manager

	JStringManager* stringMgr = JGetStringManager();	// create it
	if (defaultStringData != nullptr)
	{
		stringMgr->Register(appSignature, defaultStringData);
	}

	// user notification

	if (un != nullptr)
	{
		theUserNotification = un;
	}
	else
	{
		theUserNotification = jnew JTextUserNotification;
	}

	// progress display factory

	if (cpg != nullptr)
	{
		theCreatePG = cpg;
	}
	else
	{
		theCreatePG = jnew JCreateTextPG;
	}

	// remember to clean up

	atexit(JDeleteGlobals);
}

/******************************************************************************
 JDeleteGlobals

	Deletes all global objects and factories, in reverse order of creation.

 ******************************************************************************/

void
JDeleteGlobals()
{
	jdelete theCreatePG;
	theCreatePG = nullptr;

	jdelete theUserNotification;
	theUserNotification = nullptr;

	jdelete theStringManager;
	theStringManager = nullptr;

	jdelete theWebBrowser;
	theWebBrowser = nullptr;

	jdelete theAssertHandler;
	theAssertHandler = nullptr;

	ACE_OS::socket_fini();
}

/******************************************************************************
 JGetAssertHandler

	We cannot guarantee that JInitCore was called, so we cannot guarantee
	that we have an object that we can return.  Since assert() may be called
	because of arbitrary system failures, we cannot allocate an object
	when this function is called.  Thus, we return true if we have an
	object.

 ******************************************************************************/

bool
JGetAssertHandler
	(
	JAssertBase** ah
	)
{
	*ah = theAssertHandler;
	return *ah != nullptr;
}

/******************************************************************************
 JGetStringManager

 ******************************************************************************/

JStringManager*
JGetStringManager()
{
	if (theStringManager == nullptr)
	{
		theStringManager = jnew JStringManager;
		theStringManager->Register(nullptr, kJCoreDefaultStringData);
	}

	return theStringManager;
}

/******************************************************************************
 JGetString

	Only return a string after globals have been initialized.

 ******************************************************************************/

const JString&
JGetString
	(
	const JUtf8Byte* id
	)
{
	return (theStringManager != nullptr ? theStringManager->Get(id) : JString::empty);
}

JString
JGetString
	(
	const JUtf8Byte*	id,
	const JUtf8Byte*	map[],
	const JSize			size
	)
{
	return (theStringManager != nullptr ? theStringManager->Get(id, map, size) : JString::empty);
}

/******************************************************************************
 JGetWebBrowser

 ******************************************************************************/

JWebBrowser*
JGetWebBrowser()
{
	assert( theWebBrowser != nullptr );
	return theWebBrowser;
}

/******************************************************************************
 JSetWebBrowser

 ******************************************************************************/

void
JSetWebBrowser
	(
	JWebBrowser* webBrowser
	)
{
	assert( theWebBrowser == nullptr );
	theWebBrowser = webBrowser;
}

/******************************************************************************
 JGetUserNotification

 ******************************************************************************/

JUserNotification*
JGetUserNotification()
{
	if (theUserNotification == nullptr)
	{
		std::cerr << "Forgot to initialize UserNotification: using text version" << std::endl;

		theUserNotification = jnew JTextUserNotification;
	}

	return theUserNotification;
}

/******************************************************************************
 JGetCreatePG

 ******************************************************************************/

JCreateProgressDisplay*
JGetCreatePG()
{
	if (theCreatePG == nullptr)
	{
		std::cerr << "Forgot to initialize CreateProgressDisplay: using text version" << std::endl;

		theCreatePG = jnew JCreateTextPG;
	}

	return theCreatePG;
}

/******************************************************************************
 JSetTaskScheduler

 ******************************************************************************/

void
JSetTaskScheduler
	(
	const std::function<void(const std::function<void()>&)>& sched
	)
{
	if (theTaskScheduler != nullptr)
	{
		jdelete theTaskScheduler;
	}

	theTaskScheduler = jnew std::function(sched);
}

/******************************************************************************
 JScheduleTask

 ******************************************************************************/

void
JScheduleTask
	(
	const std::function<void()>& task
	)
{
	if (theTaskScheduler != nullptr)
	{
		(*theTaskScheduler)(task);
	}
	else
	{
		task();
	}
}

/******************************************************************************
 Interactive thread

	There is typically only one interactive thread in a program, because
	it gets too confusing otherwise.

	This is used by certain objects, e.g., JLatentPG, to decide whether
	or not to display anything.

 ******************************************************************************/

bool
JIsInteractiveThread()
{
	return theInteractiveThreadFlag;
}

void
JSetThreadIsInteractive
	(
	const bool interactive
	)
{
	theInteractiveThreadFlag = interactive;
}

/******************************************************************************
 JGetTemporaryDirectoryChangeMutex

	In a multi-threaded program, temporarily changing the working directory
	can only be done in one thread at a time.

 ******************************************************************************/

std::recursive_mutex&
JGetTemporaryDirectoryChangeMutex()
{
	return theTempDirChangeMutex;
}
