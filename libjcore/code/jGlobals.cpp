/******************************************************************************
 jGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 1997 John Lindal.

 ******************************************************************************/

#include <jGlobals.h>
#include <jStringData.h>
#include <JTextUserNotification.h>
#include <JTextChooseSaveFile.h>
#include <JCreateTextPG.h>
#include <JThisProcess.h>
#include <JWebBrowser.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <stdlib.h>
#include <locale.h>
#include <ace/OS_NS_sys_socket.h>
#include <jAssert.h>

static JAssertBase*				theAssertHandler      = NULL;

static JUserNotification*		theUserNotification    = NULL;
static JChooseSaveFile*			theChooseSaveFile      = NULL;
static JCreateProgressDisplay*	theCreatePG            = NULL;

static JStringManager*			theStringManager       = NULL;
static JWebBrowser*				theWebBrowser          = NULL;

static JString					theDefaultFontName("Helvetica", 0, kJFalse);
static JSize					theDefaultFontSize    = 10;
static JSize					theDefaultRCHFontSize = 9;

static JString					theDefaultMonospaceFontName("Courier", 0, kJFalse);
static JSize					theDefaultMonospaceFontSize = 10;

/******************************************************************************
 JInitCore

	Initializes all global objects and factories.  Calls atexit(JDeleteGlobals)
	to ensure that all objects are cleaned up when the program terminates.

	*** We take ownership of all the objects that are passed in.
		We make copies of the string data.

	Passing in NULL for ah,un,csf,cpg means that you want the default text
	implementation.

	Passing in NULL for appSignature and defaultStringData means that you want
	to use the default U.S. string data.

	Passing in NULL for gcfm,gcc means that you get nothing, so we will
	explode via assert() if these objects are requested.

	Passing in NULL for the font names means that you get:
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
	JCreateProgressDisplay*	cpg,

	const JString&			defaultFontName,
	const JSize				defaultFontSize,
	const JSize				defaultRowColHeaderFontSize,
	const JString&			defaultMonospaceFontName,
	const JSize				defaultMonospaceFontSize
	)
{
	if (theStringManager != NULL)
		{
		return;		// already initialized
		}

	// assert handler

	if (ah != NULL)
		{
		theAssertHandler = ah;
		}
	// NULL is valid in this case -- see JGetAssertHandler()

	// use native locale

	setlocale(LC_ALL, "");

	// socket library (mainly Windows)

	ACE_OS::socket_init();

	// signal handler

	JThisProcess::Initialize();

	// string manager

	JStringManager* stringMgr = JGetStringManager();	// create it
	if (defaultStringData != NULL)
		{
		stringMgr->Register(appSignature, defaultStringData);
		}

	// user notification

	if (un != NULL)
		{
		theUserNotification = un;
		}
	else
		{
		theUserNotification = jnew JTextUserNotification;
		assert( theUserNotification != NULL );
		}

	// choose/save file

	if (csf != NULL)
		{
		theChooseSaveFile = csf;
		}
	else
		{
		theChooseSaveFile = jnew JTextChooseSaveFile;
		assert( theChooseSaveFile != NULL );
		}

	// progress display factory

	if (cpg != NULL)
		{
		theCreatePG = cpg;
		}
	else
		{
		theCreatePG = jnew JCreateTextPG;
		assert( theCreatePG != NULL );
		}

	// default font

	if (stringMgr->Contains("NAME::FONT"))
		{
		theDefaultFontName = JGetString("NAME::FONT");
		}
	else if (!defaultFontName.IsEmpty())
		{
		theDefaultFontName = defaultFontName;
		}

	if (stringMgr->Contains("SIZE::FONT"))
		{
		const JString& fontSize = JGetString("SIZE::FONT");

		JSize size;
		if (fontSize.ConvertToUInt(&size))
			{
			theDefaultFontSize = size;
			}
		}
	else if (defaultFontSize > 0)
		{
		theDefaultFontSize = defaultFontSize;
		}

	if (stringMgr->Contains("SIZE::ROWCOLHDR::FONT"))
		{
		const JString& fontSize = JGetString("SIZE::ROWCOLHDR::FONT");

		JSize size;
		if (fontSize.ConvertToUInt(&size))
			{
			theDefaultRCHFontSize = size;
			}
		}
	else if (defaultRowColHeaderFontSize > 0)
		{
		theDefaultRCHFontSize = defaultRowColHeaderFontSize;
		}

	// monospace font

	if (stringMgr->Contains("NAME::MONO::FONT"))
		{
		theDefaultFontName = JGetString("NAME::MONO::FONT");
		}
	else if (!defaultMonospaceFontName.IsEmpty())
		{
		theDefaultMonospaceFontName = defaultMonospaceFontName;
		}

	if (stringMgr->Contains("SIZE::MONO::FONT"))
		{
		const JString& fontSize = JGetString("SIZE::MONO::FONT");

		JSize size;
		if (fontSize.ConvertToUInt(&size))
			{
			theDefaultMonospaceFontSize = size;
			}
		}
	else if (defaultMonospaceFontSize > 0)
		{
		theDefaultMonospaceFontSize = defaultMonospaceFontSize;
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
	theCreatePG = NULL;

	jdelete theChooseSaveFile;
	theChooseSaveFile = NULL;

	jdelete theUserNotification;
	theUserNotification = NULL;

	jdelete theStringManager;
	theStringManager = NULL;

	jdelete theWebBrowser;
	theWebBrowser = NULL;

	jdelete theAssertHandler;
	theAssertHandler = NULL;

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
	return JConvertToBoolean( *ah != NULL );
}

/******************************************************************************
 JGetStringManager

 ******************************************************************************/

JStringManager*
JGetStringManager()
{
	if (theStringManager == NULL)
		{
		theStringManager = jnew JStringManager;
		assert( theStringManager != NULL );
		theStringManager->Register(NULL, kJCoreDefaultStringData);
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
	return (theStringManager != NULL ? theStringManager->Get(id) : emptyString);
}

JString
JGetString
	(
	const JUtf8Byte*	id,
	const JUtf8Byte*	map[],
	const JSize			size
	)
{
	return (theStringManager != NULL ? theStringManager->Get(id, map, size) : emptyString);
}

/******************************************************************************
 JGetWebBrowser

 ******************************************************************************/

JWebBrowser*
JGetWebBrowser()
{
	assert( theWebBrowser != NULL );
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
	assert( theWebBrowser == NULL );
	theWebBrowser = webBrowser;
}

/******************************************************************************
 JGetUserNotification

 ******************************************************************************/

JUserNotification*
JGetUserNotification()
{
	if (theUserNotification == NULL)
		{
		std::cerr << "Forgot to initialize UserNotification: using text version" << std::endl;

		theUserNotification = jnew JTextUserNotification;
		assert( theUserNotification != NULL );
		}

	return theUserNotification;
}

/******************************************************************************
 JGetChooseSaveFile

 ******************************************************************************/

JChooseSaveFile*
JGetChooseSaveFile()
{
	if (theChooseSaveFile == NULL)
		{
		std::cerr << "Forgot to initialize ChooseSaveFile: using default version" << std::endl;

		theChooseSaveFile = jnew JTextChooseSaveFile;
		assert( theChooseSaveFile != NULL );
		}

	return theChooseSaveFile;
}

/******************************************************************************
 JGetCreatePG

 ******************************************************************************/

JCreateProgressDisplay*
JGetCreatePG()
{
	if (theCreatePG == NULL)
		{
		std::cerr << "Forgot to initialize CreateProgressDisplay: using text version" << std::endl;

		theCreatePG = jnew JCreateTextPG;
		assert( theCreatePG != NULL );
		}

	return theCreatePG;
}

/******************************************************************************
 JGetDefaultFontName

 ******************************************************************************/

const JString&
JGetDefaultFontName()
{
	return theDefaultFontName;
}

/******************************************************************************
 JGetDefaultFontSize

 ******************************************************************************/

JSize
JGetDefaultFontSize()
{
	return theDefaultFontSize;
}

/******************************************************************************
 JGetDefaultRowColHeaderFontSize

 ******************************************************************************/

JSize
JGetDefaultRowColHeaderFontSize()
{
	return theDefaultRCHFontSize;
}

/******************************************************************************
 JGetDefaultMonospaceFontName

 ******************************************************************************/

const JString&
JGetDefaultMonospaceFontName()
{
	return theDefaultMonospaceFontName;
}

/******************************************************************************
 JGetDefaultMonoFontSize

 ******************************************************************************/

JSize
JGetDefaultMonospaceFontSize()
{
	return theDefaultMonospaceFontSize;
}
