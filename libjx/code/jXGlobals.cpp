/******************************************************************************
 jXGlobals.cpp

	Access to JX global objects and factories.

	Copyright (C) 1997-99 John Lindal.

 ******************************************************************************/

#include "jXGlobals.h"
#include "JXUserNotification.h"
#include "JXChooseSaveFile.h"
#include "JXCreatePG.h"
#include "JXSearchTextDialog.h"
#include "JXSpellChecker.h"
#include "JXHelpManager.h"
#include "JXDocumentManager.h"
#include "JXDockManager.h"
#include "JXMDIServer.h"
#include "JXWebBrowser.h"
#include "JXDisplay.h"
#include "JXAssert.h"
#include "JXSharedPrefsManager.h"
#include "JXFontManager.h"
#include <jStreamUtil.h>
#include <jDirUtil.h>
#include <X11/Xlib.h>
#include <jAssert.h>

static JXApplication*			theApplication    = nullptr;	// owns itself
static JXChooseSaveFile*		theChooseSaveFile = nullptr;	// owned by JCore
static JXAssert*				theAssertHandler  = nullptr;	// owned by JCore

// we own these

static JXSharedPrefsManager*	theSharedPrefsMgr = nullptr;
static JXHelpManager*			theHelpManager    = nullptr;	// can be nullptr
static JXDocumentManager*		theDocManager     = nullptr;	// can be nullptr
static JXDockManager*			theDockManager    = nullptr;
static JXMDIServer*				theMDIServer      = nullptr;	// can be nullptr
static JXWebBrowser*			theWebBrowser     = nullptr;	// owned by JCore

static JXDirector*				thePersistentWindowOwner = nullptr;	// can be nullptr
static JXSearchTextDialog*		theSearchTextDialog      = nullptr;	// can be nullptr
static JXSpellChecker*			theSpellChecker          = nullptr;

static const JUtf8Byte* kInvisibleWindowClass = "Do_not_display_in_taskbar";
static const JUtf8Byte* kDockWindowClass      = "JX_Dock";

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
	assert( defaultStringData != nullptr );	// need JX strings

	theApplication = app;

	theAssertHandler = jnew JXAssert;
	assert( theAssertHandler != nullptr );

	auto* un = jnew JXUserNotification;
	assert( un != nullptr );

	theChooseSaveFile = jnew JXChooseSaveFile;
	assert( theChooseSaveFile != nullptr );

	JInitCore(theAssertHandler, appSignature, defaultStringData,
			  un, theChooseSaveFile, jnew JXCreatePG);

	JXFontManager::Init();

	XSetErrorHandler(JXDisplay::JXErrorHandler);
	XSetIOErrorHandler(JXApplication::JXIOErrorHandler);

	// create last so it can access as much as possible

	theSharedPrefsMgr = jnew JXSharedPrefsManager();
	assert( theSharedPrefsMgr != nullptr );

	theWebBrowser = jnew JXWebBrowser;
	assert( theWebBrowser != nullptr );
	JSetWebBrowser(theWebBrowser);		// so JGetWebBrowser() will work
}

/******************************************************************************
 JXCloseDirectors

	Called before closing JXDisplays.

 ******************************************************************************/

void
JXCloseDirectors()
{
	if (theDockManager != nullptr)
	{
		const bool ok = theDockManager->Close();
		assert( ok );

		theDockManager = nullptr;
	}

	if (thePersistentWindowOwner != nullptr)
	{
		const bool ok = thePersistentWindowOwner->Close();
		assert( ok );

		thePersistentWindowOwner = nullptr;
		theSearchTextDialog      = nullptr;
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
	theDocManager = nullptr;

	jdelete theMDIServer;
	theMDIServer = nullptr;

	jdelete theSpellChecker;
	theSpellChecker = nullptr;

	jdelete theHelpManager;
	theHelpManager = nullptr;
}

/******************************************************************************
 JXDeleteGlobals2

	Called after deleting idle and urgent tasks.

 ******************************************************************************/

void
JXDeleteGlobals2()
{
	jdelete theSharedPrefsMgr;
	theSharedPrefsMgr = nullptr;

	theChooseSaveFile = nullptr;
	theAssertHandler  = nullptr;
	theApplication    = nullptr;

//	JDeleteGlobals();	// called atexit()
}

/******************************************************************************
 JXGetApplication

 ******************************************************************************/

JXApplication*
JXGetApplication()
{
	assert( theApplication != nullptr );
	return theApplication;
}

// called by JXDisplay

bool
JXGetApplication
	(
	JXApplication** app
	)
{
	*app = theApplication;
	return theApplication != nullptr;
}

/******************************************************************************
 JXGetChooseSaveFile

 ******************************************************************************/

JXChooseSaveFile*
JXGetChooseSaveFile()
{
	assert( theChooseSaveFile != nullptr );
	return theChooseSaveFile;
}

/******************************************************************************
 JXGetAssertHandler

 ******************************************************************************/

JXAssert*
JXGetAssertHandler()
{
	assert( theAssertHandler != nullptr );
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
	assert( theDocManager != nullptr );
	return theDocManager;
}

// called by JXApplication, JXDisplay

bool
JXGetDocumentManager
	(
	JXDocumentManager** docManager
	)
{
	*docManager = theDocManager;
	return theDocManager != nullptr;
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
	assert( theDocManager == nullptr && docManager != nullptr );
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
	const bool wantShortcuts
	)
{
	assert( theDocManager == nullptr );
	auto* docManager = jnew JXDocumentManager(wantShortcuts);
	assert( docManager != nullptr );
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
	assert( theHelpManager != nullptr );
	return theHelpManager;
}

/******************************************************************************
 JXGetHelpManager

	This version returns false if the help system has not been initialized.

 ******************************************************************************/

bool
JXGetHelpManager
	(
	JXHelpManager** helpMgr
	)
{
	*helpMgr = theHelpManager;
	return theHelpManager != nullptr;
}

/******************************************************************************
 JXInitHelp

	Creates JXHelpManager.

 ******************************************************************************/

void
JXInitHelp()
{
	assert( theHelpManager == nullptr );

	theHelpManager = jnew JXHelpManager;
	assert( theHelpManager != nullptr );
}

/******************************************************************************
 JXGetDockManager

	This function calls assert() if JXSetDockManager() has not been called.

 ******************************************************************************/

JXDockManager*
JXGetDockManager()
{
	assert( theDockManager != nullptr );
	return theDockManager;
}

/******************************************************************************
 JXGetDockManager

	This version returns false if JXSetDockManager() has not been called.

 ******************************************************************************/

bool
JXGetDockManager
	(
	JXDockManager** dockMgr
	)
{
	*dockMgr = theDockManager;
	return theDockManager != nullptr;
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
	assert( theDockManager == nullptr && dockMgr != nullptr );
	theDockManager = dockMgr;
}

/******************************************************************************
 JXGetMDIServer

 ******************************************************************************/

bool
JXGetMDIServer
	(
	JXMDIServer** server
	)
{
	*server = theMDIServer;
	return theMDIServer != nullptr;
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
	assert( theMDIServer == nullptr && server != nullptr );
	theMDIServer = server;
}

/******************************************************************************
 JXGetWebBrowser

 ******************************************************************************/

JXWebBrowser*
JXGetWebBrowser()
{
	assert( theWebBrowser != nullptr );
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
	if (thePersistentWindowOwner == nullptr)
	{
		thePersistentWindowOwner = jnew JXDirector(nullptr);
		assert( thePersistentWindowOwner != nullptr );
	}

	return thePersistentWindowOwner;
}

/******************************************************************************
 JXSuspendPersistentWindows

 ******************************************************************************/

void
JXSuspendPersistentWindows()
{
	if (thePersistentWindowOwner != nullptr)
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
	if (thePersistentWindowOwner != nullptr)
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
	if (theSearchTextDialog == nullptr)
	{
		JXSearchTextDialog::Create();
	}

	// The dialog must register itself.

	assert( theSearchTextDialog != nullptr );
	return theSearchTextDialog;
}

bool
JXGetSearchTextDialog
	(
	JXSearchTextDialog** dlog
	)
{
	*dlog = theSearchTextDialog;
	return theSearchTextDialog != nullptr;
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
	assert( theSearchTextDialog == nullptr && dlog != nullptr );
	theSearchTextDialog = dlog;
}

/******************************************************************************
 JXGetSpellChecker

 ******************************************************************************/

JXSpellChecker*
JXGetSpellChecker()
{
	if (theSpellChecker == nullptr)
	{
		theSpellChecker = jnew JXSpellChecker();
	}

	assert( theSpellChecker != nullptr );
	return theSpellChecker;
}

/******************************************************************************
 JXGetProgramDataDirectories

	Returns the full paths of the system and user directories in which the
	program should create its private directories.  dirName is appended to
	each one for convenience.

	Returns false if the user doesn't have a home directory, in which
	case userDir is empty.

	*** Does not check if either directory exists.

 ******************************************************************************/

bool
JXGetProgramDataDirectories
	(
	const JUtf8Byte*	dirName,
	JString*			sysDir,
	JString*			userDir
	)
{
	return JGetDataDirectories(JXGetApplication()->GetSignature().GetBytes(),
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
