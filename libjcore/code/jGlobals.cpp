/******************************************************************************
 jGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 1997 John Lindal.

 ******************************************************************************/

#include "jGlobals.h"
#include "jStringData.h"
#include "JTextUserNotification.h"
#include "JTextChooseSaveFile.h"
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
static JChooseSaveFile*			theChooseSaveFile   = nullptr;
static JCreateProgressDisplay*	theCreatePG         = nullptr;

static JStringManager*			theStringManager    = nullptr;
static JWebBrowser*				theWebBrowser       = nullptr;

/******************************************************************************
 JInitCore

	Initializes all global objects and factories.  Calls atexit(JDeleteGlobals)
	to ensure that all objects are cleaned up when the program terminates.

	*** We take ownership of all the objects that are passed in.
		We make copies of the string data.

	Passing in nullptr for ah,un,csf,cpg means that you want the default text
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
	JChooseSaveFile*		csf,
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
		assert( theUserNotification != nullptr );
		}

	// choose/save file

	if (csf != nullptr)
		{
		theChooseSaveFile = csf;
		}
	else
		{
		theChooseSaveFile = jnew JTextChooseSaveFile;
		assert( theChooseSaveFile != nullptr );
		}

	// progress display factory

	if (cpg != nullptr)
		{
		theCreatePG = cpg;
		}
	else
		{
		theCreatePG = jnew JCreateTextPG;
		assert( theCreatePG != nullptr );
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

	jdelete theChooseSaveFile;
	theChooseSaveFile = nullptr;

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
	when this function is called.  Thus, we return kJTrue if we have an
	object.

 ******************************************************************************/

JBoolean
JGetAssertHandler
	(
	JAssertBase** ah
	)
{
	*ah = theAssertHandler;
	return JConvertToBoolean( *ah != nullptr );
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
		assert( theStringManager != nullptr );
		theStringManager->Register(nullptr, kJCoreDefaultStringData);
		}

	return theStringManager;
}

/******************************************************************************
 JGetString

	Only return a string after globals have been initialized.

 ******************************************************************************/

static const JString emptyString;

const JString&
JGetString
	(
	const JUtf8Byte* id
	)
{
	return (theStringManager != nullptr ? theStringManager->Get(id) : emptyString);
}

JString
JGetString
	(
	const JUtf8Byte*	id,
	const JUtf8Byte*	map[],
	const JSize			size
	)
{
	return (theStringManager != nullptr ? theStringManager->Get(id, map, size) : emptyString);
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
		assert( theUserNotification != nullptr );
		}

	return theUserNotification;
}

/******************************************************************************
 JGetChooseSaveFile

 ******************************************************************************/

JChooseSaveFile*
JGetChooseSaveFile()
{
	if (theChooseSaveFile == nullptr)
		{
		std::cerr << "Forgot to initialize ChooseSaveFile: using default version" << std::endl;

		theChooseSaveFile = jnew JTextChooseSaveFile;
		assert( theChooseSaveFile != nullptr );
		}

	return theChooseSaveFile;
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
		assert( theCreatePG != nullptr );
		}

	return theCreatePG;
}
