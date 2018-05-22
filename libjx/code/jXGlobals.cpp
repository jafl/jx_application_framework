/******************************************************************************
 jXGlobals.cpp

	Access to JX global objects and factories.

	Copyright (C) 1997-99 John Lindal.

 ******************************************************************************/

#include <jXGlobals.h>
#include <JXUserNotification.h>
#include <JXChooseSaveFile.h>
#include <JXCreatePG.h>
#include <JXSearchTextDialog.h>
#include <JXSpellChecker.h>
#include <JXHelpManager.h>
#include <JXDocumentManager.h>
#include <JXDockManager.h>
#include <JXMDIServer.h>
#include <JXWebBrowser.h>
#include <JXDisplay.h>
#include <JXAssert.h>
#include <JXSharedPrefsManager.h>
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <X11/Xlib.h>
#include <jAssert.h>

static JXApplication*			theApplication    = NULL;	// owns itself
static JXChooseSaveFile*		theChooseSaveFile = NULL;	// owned by JCore
static JXAssert*				theAssertHandler  = NULL;	// owned by JCore

// we own these

static JXSharedPrefsManager*	theSharedPrefsMgr = NULL;
static JXHelpManager*			theHelpManager    = NULL;	// can be NULL
static JXDocumentManager*		theDocManager     = NULL;	// can be NULL
static JXDockManager*			theDockManager    = NULL;
static JXMDIServer*				theMDIServer      = NULL;	// can be NULL
static JXWebBrowser*			theWebBrowser     = NULL;	// owned by JCore

static JXDirector*				thePersistentWindowOwner  = NULL;	// can be NULL
static JXSearchTextDialog*		theSearchTextDialog       = NULL;	// can be NULL
static JXSpellChecker*			theSpellChecker           = NULL;

static JXComposeRuleList*		theComposeRuleList        = NULL;	// can be NULL

static const JUtf8Byte* kInvisibleWindowClass = "Do_not_display_in_taskbar";
static const JUtf8Byte* kDockWindowClass      = "JX_Dock";

static const JString kDefaultFontName(
	#ifdef _J_OSX
	"Arial",
	#else
	"Helvetica",
	#endif
	0, kJFalse);

static const JString kMonospaceFontName("Bitstream Vera Sans Mono", 0, kJFalse);

/******************************************************************************
 JXCreateGlobals

 ******************************************************************************/

void
JXCreateGlobals
	(
	JXApplication*		app,
	const JUtf8Byte*	appSignature,
	const JUtf8Byte**	defaultStringData
	)
{
	assert( defaultStringData != NULL );	// need JX strings

	theApplication = app;

	theAssertHandler = jnew JXAssert;
	assert( theAssertHandler != NULL );

	JXUserNotification* un = jnew JXUserNotification;
	assert( un != NULL );

	theChooseSaveFile = jnew JXChooseSaveFile;
	assert( theChooseSaveFile != NULL );

	JInitCore(theAssertHandler, appSignature, defaultStringData,
			  un, theChooseSaveFile, jnew JXCreatePG,
			  kDefaultFontName, 0, 0, kMonospaceFontName, 0);

	XSetErrorHandler(JXDisplay::JXErrorHandler);
	XSetIOErrorHandler(JXApplication::JXIOErrorHandler);

	// create last so it can access as much as possible

	theSharedPrefsMgr = jnew JXSharedPrefsManager();
	assert( theSharedPrefsMgr != NULL );

	theWebBrowser = jnew JXWebBrowser;
	assert( theWebBrowser != NULL );
	JSetWebBrowser(theWebBrowser);		// so JGetWebBrowser() will work
}

/******************************************************************************
 JXCloseDirectors

	Called before closing JXDisplays.

 ******************************************************************************/

void
JXCloseDirectors()
{
	if (theDockManager != NULL)
		{
		const JBoolean ok = theDockManager->Close();
		assert( ok );

		theDockManager = NULL;
		}

	if (thePersistentWindowOwner != NULL)
		{
		const JBoolean ok = thePersistentWindowOwner->Close();
		assert( ok );

		thePersistentWindowOwner = NULL;
		theSearchTextDialog      = NULL;
		}
}

/******************************************************************************
 JXDeleteGlobals1

	Called after closing JXDisplays, before deleting idle and urgent tasks.

 ******************************************************************************/

void
JXDeleteGlobals1()
{
	jdelete theDocManager;
	theDocManager = NULL;

	jdelete theMDIServer;
	theMDIServer = NULL;

	jdelete theSpellChecker;
	theSpellChecker = NULL;

	jdelete theHelpManager;
	theHelpManager = NULL;
}

/******************************************************************************
 JXDeleteGlobals2

	Called after deleting idle and urgent tasks.

 ******************************************************************************/

void
JXDeleteGlobals2()
{
	jdelete theSharedPrefsMgr;
	theSharedPrefsMgr = NULL;

	theChooseSaveFile = NULL;
	theAssertHandler  = NULL;
	theApplication    = NULL;

//	JDeleteGlobals();	// called atexit()
}

/******************************************************************************
 JXGetApplication

 ******************************************************************************/

JXApplication*
JXGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

// called by JXDisplay

JBoolean
JXGetApplication
	(
	JXApplication** app
	)
{
	*app = theApplication;
	return JConvertToBoolean( theApplication != NULL );
}

/******************************************************************************
 JXGetChooseSaveFile

 ******************************************************************************/

JXChooseSaveFile*
JXGetChooseSaveFile()
{
	assert( theChooseSaveFile != NULL );
	return theChooseSaveFile;
}

/******************************************************************************
 JXGetAssertHandler

 ******************************************************************************/

JXAssert*
JXGetAssertHandler()
{
	assert( theAssertHandler != NULL );
	return theAssertHandler;
}

/******************************************************************************
 JXGetDocumentManager

	This function calls assert() if JXSetDocumentManager() has not been called.
	We do this because JXDocuments cannot be used otherwise.

 ******************************************************************************/

JXDocumentManager*
JXGetDocumentManager()
{
	assert( theDocManager != NULL );
	return theDocManager;
}

// called by JXApplication, JXDisplay

JBoolean
JXGetDocumentManager
	(
	JXDocumentManager** docManager
	)
{
	*docManager = theDocManager;
	return JConvertToBoolean( theDocManager != NULL );
}

/******************************************************************************
 JXSetDocumentManager

	The application code must call this exactly once if it uses JXDocument.
	We don't create JXDocumentManager automatically because one may need
	a derived class.

	After calling this function, we own the object and will delete it when
	the program terminates.

 ******************************************************************************/

void
JXSetDocumentManager
	(
	JXDocumentManager* docManager
	)
{
	assert( theDocManager == NULL && docManager != NULL );
	theDocManager = docManager;
}

/******************************************************************************
 JXCreateDefaultDocumentManager

	This is provided for convenience for all the programs that do
	not need a derived class of JXDocumentManager.

 ******************************************************************************/

void
JXCreateDefaultDocumentManager
	(
	const JBoolean wantShortcuts
	)
{
	assert( theDocManager == NULL );
	JXDocumentManager* docManager = jnew JXDocumentManager(wantShortcuts);
	assert( docManager != NULL );
	// constructor calls JXSetDocumentManager()
}

/******************************************************************************
 JXGetHelpManager

	This function calls assert() if JXInitHelp() has not been called.
	We do this because the help system cannot be used otherwise.

 ******************************************************************************/

JXHelpManager*
JXGetHelpManager()
{
	assert( theHelpManager != NULL );
	return theHelpManager;
}

/******************************************************************************
 JXGetHelpManager

	This version returns kJFalse if the help system has not been initialized.

 ******************************************************************************/

JBoolean
JXGetHelpManager
	(
	JXHelpManager** helpMgr
	)
{
	*helpMgr = theHelpManager;
	return JI2B( theHelpManager != NULL );
}

/******************************************************************************
 JXInitHelp

	Creates JXHelpManager.

 ******************************************************************************/

void
JXInitHelp()
{
	assert( theHelpManager == NULL );

	theHelpManager = jnew JXHelpManager;
	assert( theHelpManager != NULL );
}

/******************************************************************************
 JXGetDockManager

	This function calls assert() if JXSetDockManager() has not been called.

 ******************************************************************************/

JXDockManager*
JXGetDockManager()
{
	assert( theDockManager != NULL );
	return theDockManager;
}

/******************************************************************************
 JXGetDockManager

	This version returns kJFalse if JXSetDockManager() has not been called.

 ******************************************************************************/

JBoolean
JXGetDockManager
	(
	JXDockManager** dockMgr
	)
{
	*dockMgr = theDockManager;
	return JI2B( theDockManager != NULL );
}

/******************************************************************************
 JXSetDockManager

	Automatically called by JXMDIServer.

	After calling this function, we own the object and will delete it when
	the program terminates.

 ******************************************************************************/

void
JXSetDockManager
	(
	JXDockManager* dockMgr
	)
{
	assert( theDockManager == NULL && dockMgr != NULL );
	theDockManager = dockMgr;
}

/******************************************************************************
 JXGetMDIServer

 ******************************************************************************/

JBoolean
JXGetMDIServer
	(
	JXMDIServer** server
	)
{
	*server = theMDIServer;
	return JConvertToBoolean( theMDIServer != NULL );
}

/******************************************************************************
 JXSetMDIServer

	Automatically called by JXMDIServer.

	After calling this function, we own the object and will delete it when
	the program terminates.

 ******************************************************************************/

void
JXSetMDIServer
	(
	JXMDIServer* server
	)
{
	assert( theMDIServer == NULL && server != NULL );
	theMDIServer = server;
}

/******************************************************************************
 JXGetWebBrowser

 ******************************************************************************/

JXWebBrowser*
JXGetWebBrowser()
{
	assert( theWebBrowser != NULL );
	return theWebBrowser;
}

/******************************************************************************
 JXGetSharedPrefsManager

 ******************************************************************************/

JXSharedPrefsManager*
JXGetSharedPrefsManager()
{
	return theSharedPrefsMgr;
}

/******************************************************************************
 JXGetPersistentWindowOwner

	This should be the supervisor of all persistent windows whose existence
	should not keep the application running.

 ******************************************************************************/

JXDirector*
JXGetPersistentWindowOwner()
{
	if (thePersistentWindowOwner == NULL)
		{
		thePersistentWindowOwner = jnew JXDirector(NULL);
		assert( thePersistentWindowOwner != NULL );
		}

	return thePersistentWindowOwner;
}

/******************************************************************************
 JXSuspendPersistentWindows

 ******************************************************************************/

void
JXSuspendPersistentWindows()
{
	if (thePersistentWindowOwner != NULL)
		{
		thePersistentWindowOwner->Suspend();
		}
}

/******************************************************************************
 JXResumePersistentWindows

 ******************************************************************************/

void
JXResumePersistentWindows()
{
	if (thePersistentWindowOwner != NULL)
		{
		thePersistentWindowOwner->Resume();
		}
}

/******************************************************************************
 JXGetSearchTextDialog

 ******************************************************************************/

JXSearchTextDialog*
JXGetSearchTextDialog()
{
	if (theSearchTextDialog == NULL)
		{
		JXSearchTextDialog::Create();
		}

	// The dialog must register itself.

	assert( theSearchTextDialog != NULL );
	return theSearchTextDialog;
}

JBoolean
JXGetSearchTextDialog
	(
	JXSearchTextDialog** dlog
	)
{
	*dlog = theSearchTextDialog;
	return JI2B( theSearchTextDialog != NULL );
}

/******************************************************************************
 JXSetSearchTextDialog

	Automatically called by JXSearchTextDialog.

	After calling this function, we own the object and will delete it when
	the program terminates.

 ******************************************************************************/

void
JXSetSearchTextDialog
	(
	JXSearchTextDialog* dlog
	)
{
	assert( theSearchTextDialog == NULL && dlog != NULL );
	theSearchTextDialog = dlog;
}

/******************************************************************************
 JXGetSpellChecker

 ******************************************************************************/

JXSpellChecker*
JXGetSpellChecker()
{
	if (theSpellChecker == NULL)
		{
		theSpellChecker = jnew JXSpellChecker();
		}

	assert( theSpellChecker != NULL );
	return theSpellChecker;
}

/******************************************************************************
 JXGetProgramDataDirectories

	Returns the full paths of the system and user directories in which the
	program should create its private directories.  dirName is appended to
	each one for convenience.

	Returns kJFalse if the user doesn't have a home directory, in which
	case userDir is empty.

	*** Does not check if either directory exists.

 ******************************************************************************/

JBoolean
JXGetProgramDataDirectories
	(
	const JString&	dirName,
	JString*		sysDir,
	JString*		userDir
	)
{
	return JGetDataDirectories((JXGetApplication())->GetSignature(),
							   dirName, sysDir, userDir);
}

/******************************************************************************
 JXGetInvisibleWindowClass

 ******************************************************************************/

const JUtf8Byte*
JXGetInvisibleWindowClass()
{
	return kInvisibleWindowClass;
}

/******************************************************************************
 JXGetDockWindowClass

 ******************************************************************************/

const JUtf8Byte*
JXGetDockWindowClass()
{
	return kDockWindowClass;
}
