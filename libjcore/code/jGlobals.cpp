/******************************************************************************
 jGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 1997 John Lindal. All rights reserved.

 ******************************************************************************/

#include <jGlobals.h>
#include <jStringData.h>
#include <JTextUserNotification.h>
#include <JTextChooseSaveFile.h>
#include <JCreateTextPG.h>
#include <JGetCurrentFontManager.h>
#include <JGetCurrentColormap.h>
#include <JThisProcess.h>
#include <JWebBrowser.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <stdlib.h>
#include <j_prep_ace.h>
#include <ace/OS_NS_sys_socket.h>
#include <jAssert.h>

static JAssertBase*				theAssertHandler     = NULL;

static JUserNotification*		theUserNotification  = NULL;
static JChooseSaveFile*			theChooseSaveFile    = NULL;
static JCreateProgressDisplay*	theCreatePG          = NULL;

static JGetCurrentFontManager*	theGetCurrFontMgr    = NULL;
static JGetCurrentColormap*		theGetCurrColormap   = NULL;

static JStringManager*			theStringManager     = NULL;
static JWebBrowser*				theWebBrowser        = NULL;

static JString*					theDefaultFontName   = NULL;
static JString*					theGreekFontName     = NULL;
static JString*					theMonospaceFontName = NULL;

static const JCharacter* kDefaultFontName   = "Helvetica";
static const JCharacter* kGreekFontName     = "Symbol";
static const JCharacter* kMonospaceFontName = "Courier";

/******************************************************************************
 JInitCore

	Initializes all global objects and factories.  Calls atexit(JDeleteGlobals)
	to insure that all objects are cleaned up when the program terminates.

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

	const JCharacter*		appSignature,
	const JCharacter**		defaultStringData,

	JUserNotification*		un,
	JChooseSaveFile*		csf,
	JCreateProgressDisplay*	cpg,

	JGetCurrentFontManager*	gcfm,
	JGetCurrentColormap*	gcc,

	const JCharacter*		defaultFontName,
	const JCharacter*		greekFontName,
	const JCharacter*		monospaceFontName
	)
{
	// assert handler

	if (ah != NULL)
		{
		theAssertHandler = ah;
		}
	// NULL is valid in this case -- see JGetAssertHandler()

	// socket library (mainly Windows)

	ACE_OS::socket_init();

	// signal handler

	JThisProcess::Initialize();

	// initialize write-once/read-many static data for thread safety

	JString s;
	JGetTempDirectory(&s);

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
		theUserNotification = new JTextUserNotification;
		assert( theUserNotification != NULL );
		}

	// choose/save file

	if (csf != NULL)
		{
		theChooseSaveFile = csf;
		}
	else
		{
		theChooseSaveFile = new JTextChooseSaveFile;
		assert( theChooseSaveFile != NULL );
		}

	// progress display factory

	if (cpg != NULL)
		{
		theCreatePG = cpg;
		}
	else
		{
		theCreatePG = new JCreateTextPG;
		assert( theCreatePG != NULL );
		}

	// font manager

	if (gcfm != NULL)
		{
		theGetCurrFontMgr = gcfm;
		}

	// colormap

	if (gcc != NULL)
		{
		theGetCurrColormap = gcc;
		}

	// default font name

	if (!JStringEmpty(defaultFontName))
		{
		theDefaultFontName = new JString(defaultFontName);
		assert( theDefaultFontName != NULL );
		}

	// greek font name

	if (!JStringEmpty(greekFontName))
		{
		theGreekFontName = new JString(greekFontName);
		assert( theGreekFontName != NULL );
		}

	// monospace font name

	if (!JStringEmpty(monospaceFontName))
		{
		theMonospaceFontName = new JString(monospaceFontName);
		assert( theMonospaceFontName != NULL );
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
	delete theDefaultFontName;
	theDefaultFontName = NULL;

	delete theGreekFontName;
	theGreekFontName = NULL;

	delete theMonospaceFontName;
	theMonospaceFontName = NULL;

	delete theGetCurrColormap;
	theGetCurrColormap = NULL;

	delete theGetCurrFontMgr;
	theGetCurrFontMgr = NULL;

	delete theCreatePG;
	theCreatePG = NULL;

	delete theChooseSaveFile;
	theChooseSaveFile = NULL;

	delete theUserNotification;
	theUserNotification = NULL;

	delete theStringManager;
	theStringManager = NULL;

	delete theWebBrowser;
	theWebBrowser = NULL;

	delete theAssertHandler;
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
		theStringManager = new JStringManager;
		assert( theStringManager != NULL );
		theStringManager->Register(NULL, kJCoreDefaultStringData);
		}

	return theStringManager;
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
		cerr << "Forgot to initialize UserNotification: using text version" << endl;

		theUserNotification = new JTextUserNotification;
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
		cerr << "Forgot to initialize ChooseSaveFile: using default version" << endl;

		theChooseSaveFile = new JTextChooseSaveFile;
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
		cerr << "Forgot to initialize CreateProgressDisplay: using text version" << endl;

		theCreatePG = new JCreateTextPG;
		assert( theCreatePG != NULL );
		}

	return theCreatePG;
}

/******************************************************************************
 JGetCurrFontManager

 ******************************************************************************/

const JFontManager*
JGetCurrFontManager()
{
	assert( theGetCurrFontMgr != NULL );
	return theGetCurrFontMgr->GetCurrFontManager();
}

/******************************************************************************
 JGetCurrColormap

 ******************************************************************************/

JColormap*
JGetCurrColormap()
{
	assert( theGetCurrColormap != NULL );
	return theGetCurrColormap->GetCurrColormap();
}

/******************************************************************************
 JGetDefaultFontName

 ******************************************************************************/

const JCharacter*
JGetDefaultFontName()
{
	return (theDefaultFontName == NULL ? kDefaultFontName : theDefaultFontName->GetCString());
}

/******************************************************************************
 JGetGreekFontName

 ******************************************************************************/

const JCharacter*
JGetGreekFontName()
{
	return (theGreekFontName == NULL ? kGreekFontName : theGreekFontName->GetCString());
}

/******************************************************************************
 JGetMonospaceFontName

 ******************************************************************************/

const JCharacter*
JGetMonospaceFontName()
{
	return (theMonospaceFontName == NULL ? kMonospaceFontName : theMonospaceFontName->GetCString());
}

/******************************************************************************
 JGetJDataDirectories

	Returns the full paths of the system and user directories in which
	JX objects should create their private directories.  dirName is
	appended to each one for convenience.

	Returns kJFalse if the user doesn't have a home directory, in which
	case userDir is empty.

	*** Does not check if either directory exists.

 ******************************************************************************/

JBoolean
JGetJDataDirectories
	(
	const JCharacter*	dirName,
	JString*			sysDir,
	JString*			userDir
	)
{
	return JGetDataDirectories("jx", dirName, sysDir, userDir);
}
