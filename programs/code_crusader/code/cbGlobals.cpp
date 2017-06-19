/******************************************************************************
 cbGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 1997-99 John Lindal. All rights reserved.

 ******************************************************************************/

#include "cbGlobals.h"
#include "CBMDIServer.h"
#include "CBDockManager.h"
#include "CBSearchTextDialog.h"
#include "CBRunTEScriptDialog.h"
#include "CBViewManPageDialog.h"
#include "CBFindFileDialog.h"
#include "CBDiffFileDialog.h"
#include "CBCommandManager.h"
#include "CBFnMenuUpdater.h"
#include "CBSymbolTypeList.h"
#include "CBPTPrinter.h"
#include "CBPSPrinter.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXImageCache.h>
#include <JXImage.h>
#include <jAssert.h>

static CBApp*				theApplication       = NULL;	// owns itself
static CBDocumentManager*	theDocManager        = NULL;	// owned by JX
static CBPrefsManager*		thePrefsManager      = NULL;
static CBMDIServer*			theMDIServer         = NULL;	// owned by JX

static CBSearchTextDialog*	theSearchTextDialog  = NULL;	// owned by JXGetPersistentWindowOwner()
static CBRunTEScriptDialog*	theRunTEScriptDialog = NULL;	// owned by JXGetPersistentWindowOwner()
static CBViewManPageDialog*	theViewManPageDialog = NULL;	// owned by JXGetPersistentWindowOwner()
static CBFindFileDialog*	theFindFileDialog    = NULL;	// owned by JXGetPersistentWindowOwner()
static CBDiffFileDialog*	theDiffFileDialog    = NULL;	// owned by JXGetPersistentWindowOwner()

static CBCommandManager*	theCmdManager        = NULL;
static CBFnMenuUpdater*		theFnMenuUpdater     = NULL;
static CBSymbolTypeList*	theSymbolTypeList    = NULL;

static CBPTPrinter*			thePTTextPrinter     = NULL;
static CBPSPrinter*			thePSTextPrinter     = NULL;

static const JCharacter* kProjectWindowClass       = "Code_Crusader_Project";
static const JCharacter* kSymbolWindowClass        = "Code_Crusader_Symbol_List";
static const JCharacter* kTreeWindowClass          = "Code_Crusader_Tree";
static const JCharacter* kFileListWindowClass      = "Code_Crusader_File_List";
static const JCharacter* kEditorWindowClass        = "Code_Crusader_Editor";
static const JCharacter* kExecOutputWindowClass    = "Code_Crusader_Editor_Exec_Output";
static const JCharacter* kCompileOutputWindowClass = "Code_Crusader_Editor_Compile_Output";
static const JCharacter* kSearchOutputWindowClass  = "Code_Crusader_Editor_Search_Output";
static const JCharacter* kShellWindowClass         = "Code_Crusader_Editor_Shell";

// owned by JXImageCache
static JXImage* theFileIcon           = NULL;
static JXImage* theWritableFileIcon   = NULL;
static JXImage* theSourceIcon         = NULL;
static JXImage* theWritableSourceIcon = NULL;
static JXImage* theLibraryIcon        = NULL;
static JXImage* theProjectIcon        = NULL;
static JXImage* theActiveProjectIcon  = NULL;
static JXImage* theActiveListIcon     = NULL;

static JBoolean theIsUpdateThread = kJFalse;

// private functions

void	CBCreateIcons();
void	CBDeleteIcons();

// string ID's

static const JCharacter* kDescriptionID = "Description::cbGlobals";

/******************************************************************************
 CBCreateGlobals

	Returns kJTrue if this is the first time the program is run.

 ******************************************************************************/

JBoolean
CBCreateGlobals
	(
	CBApp*			app,
	const JBoolean	useMDI
	)
{
	if (JAssertBase::GetAction() == JAssertBase::kAskUser)
		{
		JAssertBase::SetAction(JAssertBase::kAbort);
		}

	theApplication = app;

	CBCreateIcons();

	// prefs must be created first so everybody can read from it

	JBoolean isNew;
	thePrefsManager = jnew CBPrefsManager(&isNew);
	assert( thePrefsManager != NULL );

	JXInitHelp();

	theDocManager = jnew CBDocumentManager;
	assert( theDocManager != NULL );

	if (useMDI)
		{
		CBDockManager* dockManager = jnew CBDockManager();
		assert( dockManager != NULL );
		dockManager->JPrefObject::ReadPrefs();

		theMDIServer = jnew CBMDIServer;
		assert( theMDIServer != NULL );
		}

	theSearchTextDialog = CBSearchTextDialog::Create();
	assert( theSearchTextDialog != NULL );

	theRunTEScriptDialog = jnew CBRunTEScriptDialog(JXGetPersistentWindowOwner());
	assert( theRunTEScriptDialog != NULL );

	theViewManPageDialog = jnew CBViewManPageDialog(JXGetPersistentWindowOwner());
	assert( theViewManPageDialog != NULL );

	theFindFileDialog = jnew CBFindFileDialog(JXGetPersistentWindowOwner());
	assert( theFindFileDialog != NULL );

	theDiffFileDialog = jnew CBDiffFileDialog(JXGetPersistentWindowOwner());
	assert( theDiffFileDialog != NULL );

	// widgets hidden in permanent window

	JXWindow* permWindow = theSearchTextDialog->GetWindow();

	theDocManager->CreateFileHistoryMenus(permWindow);

	// global commands

	theCmdManager = jnew CBCommandManager(theDocManager);
	assert( theCmdManager != NULL );
	theCmdManager->JPrefObject::ReadPrefs();

	// fn menu updater

	theFnMenuUpdater = jnew CBFnMenuUpdater;
	assert( theFnMenuUpdater != NULL );

	// symbol type list

	theSymbolTypeList = jnew CBSymbolTypeList(permWindow->GetColormap());
	assert( theSymbolTypeList != NULL );

	// printers

	thePTTextPrinter = jnew CBPTPrinter;
	assert( thePTTextPrinter != NULL );

	thePSTextPrinter = jnew CBPSPrinter(permWindow->GetDisplay());
	assert( thePSTextPrinter != NULL );

	return isNew;
}

/******************************************************************************
 CBDeleteGlobals

 ******************************************************************************/

void
CBDeleteGlobals()
{
	theDocManager->JPrefObject::WritePrefs();

	JXDockManager* dockMgr;
	if (JXGetDockManager(&dockMgr))
		{
		dockMgr->JPrefObject::WritePrefs();
		}

	if (theMDIServer != NULL)
		{
		theMDIServer->JPrefObject::WritePrefs();
		}

	CBDeleteIcons();

	theApplication = NULL;
	theDocManager  = NULL;
	theMDIServer   = NULL;

	theCmdManager->JPrefObject::WritePrefs();
	jdelete theCmdManager;
	theCmdManager = NULL;

	jdelete theFnMenuUpdater;
	theFnMenuUpdater = NULL;

	jdelete theSymbolTypeList;
	theSymbolTypeList = NULL;

	// printers

	jdelete thePTTextPrinter;
	thePTTextPrinter = NULL;

	jdelete thePSTextPrinter;
	thePSTextPrinter = NULL;

	CBShutdownCompleters();
	CBShutdownStylers();

	// windows closed by JXGetPersistentWindowOwner()

	theSearchTextDialog->JPrefObject::WritePrefs();
	theRunTEScriptDialog->JPrefObject::WritePrefs();
	theViewManPageDialog->JPrefObject::WritePrefs();
	theFindFileDialog->JPrefObject::WritePrefs();
	theDiffFileDialog->JPrefObject::WritePrefs();

	theSearchTextDialog  = NULL;
	theRunTEScriptDialog = NULL;
	theViewManPageDialog = NULL;
	theFindFileDialog    = NULL;
	theDiffFileDialog    = NULL;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = NULL;
}

/******************************************************************************
 CBCleanUpBeforeSuddenDeath

	This must be the last one called by CBApp so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
CBCleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
		theSearchTextDialog->JPrefObject::WritePrefs();
		theRunTEScriptDialog->JPrefObject::WritePrefs();
		theViewManPageDialog->JPrefObject::WritePrefs();
		theFindFileDialog->JPrefObject::WritePrefs();
		theDiffFileDialog->JPrefObject::WritePrefs();

		theDocManager->JPrefObject::WritePrefs();

		if (theMDIServer != NULL)
			{
			theMDIServer->JPrefObject::WritePrefs();
			}

		CBShutdownCompleters();
		CBShutdownStylers();
		}

	// must be last to save everything

	thePrefsManager->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 CBGetApplication

 ******************************************************************************/

CBApp*
CBGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 CBInUpdateThread

 ******************************************************************************/

JBoolean
CBInUpdateThread()
{
	return theIsUpdateThread;
}

/******************************************************************************
 CBSetUpdateThread

 ******************************************************************************/

void
CBSetUpdateThread()
{
	theIsUpdateThread = kJTrue;
}

/******************************************************************************
 CBGetDocumentManager

 ******************************************************************************/

CBDocumentManager*
CBGetDocumentManager()
{
	assert( theDocManager != NULL );
	return theDocManager;
}

/******************************************************************************
 CBGetPrefsManager

 ******************************************************************************/

CBPrefsManager*
CBGetPrefsManager()
{
	assert( thePrefsManager != NULL );
	return thePrefsManager;
}

/******************************************************************************
 CBGetMDIServer

 ******************************************************************************/

JBoolean
CBGetMDIServer
	(
	CBMDIServer** mdi
	)
{
	*mdi = theMDIServer;
	return JI2B( theMDIServer != NULL );
}

/******************************************************************************
 CBGetSearchTextDialog

 ******************************************************************************/

CBSearchTextDialog*
CBGetSearchTextDialog()
{
	assert( theSearchTextDialog != NULL );
	return theSearchTextDialog;
}

/******************************************************************************
 CBGetRunTEScriptDialog

 ******************************************************************************/

CBRunTEScriptDialog*
CBGetRunTEScriptDialog()
{
	assert( theRunTEScriptDialog != NULL );
	return theRunTEScriptDialog;
}

/******************************************************************************
 CBGetViewManPageDialog

 ******************************************************************************/

CBViewManPageDialog*
CBGetViewManPageDialog()
{
	assert( theViewManPageDialog != NULL );
	return theViewManPageDialog;
}

/******************************************************************************
 CBGetFindFileDialog

 ******************************************************************************/

CBFindFileDialog*
CBGetFindFileDialog()
{
	assert( theFindFileDialog != NULL );
	return theFindFileDialog;
}

/******************************************************************************
 CBGetDiffFileDialog

 ******************************************************************************/

CBDiffFileDialog*
CBGetDiffFileDialog()
{
	assert( theDiffFileDialog != NULL );
	return theDiffFileDialog;
}

/******************************************************************************
 CBGetCommandManager

 ******************************************************************************/

CBCommandManager*
CBGetCommandManager()
{
	assert( theCmdManager != NULL );
	return theCmdManager;
}

/******************************************************************************
 CBGetFnMenuUpdater

 ******************************************************************************/

CBFnMenuUpdater*
CBGetFnMenuUpdater()
{
	assert( theFnMenuUpdater != NULL );
	return theFnMenuUpdater;
}

/******************************************************************************
 CBGetSymbolTypeList

 ******************************************************************************/

CBSymbolTypeList*
CBGetSymbolTypeList()
{
	assert( theSymbolTypeList != NULL );
	return theSymbolTypeList;
}

/******************************************************************************
 CBGetPTTextPrinter

 ******************************************************************************/

CBPTPrinter*
CBGetPTTextPrinter()
{
	assert( thePTTextPrinter != NULL );
	return thePTTextPrinter;
}

/******************************************************************************
 CBGetPSTextPrinter

 ******************************************************************************/

CBPSPrinter*
CBGetPSTextPrinter()
{
	assert( thePSTextPrinter != NULL );
	return thePSTextPrinter;
}

/******************************************************************************
 CBGetWMClassInstance

 ******************************************************************************/

const JCharacter*
CBGetWMClassInstance()
{
	return JGetString("CBName");
}

const JCharacter*
CBGetProjectWindowClass()
{
	return kProjectWindowClass;
}

const JCharacter*
CBGetSymbolWindowClass()
{
	return kSymbolWindowClass;
}

const JCharacter*
CBGetTreeWindowClass()
{
	return kTreeWindowClass;
}

const JCharacter*
CBGetFileListWindowClass()
{
	return kFileListWindowClass;
}

const JCharacter*
CBGetEditorWindowClass()
{
	return kEditorWindowClass;
}

const JCharacter*
CBGetExecOutputWindowClass()
{
	return kExecOutputWindowClass;
}

const JCharacter*
CBGetCompileOutputWindowClass()
{
	return kCompileOutputWindowClass;
}

const JCharacter*
CBGetSearchOutputWindowClass()
{
	return kSearchOutputWindowClass;
}

const JCharacter*
CBGetShellWindowClass()
{
	return kShellWindowClass;
}

/******************************************************************************
 CBGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
CBGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 CBGetVersionStr

 ******************************************************************************/

JString
CBGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}

/******************************************************************************
 Icons

 ******************************************************************************/

#include "jcc_plain_file_small.xpm"
#include "jcc_plain_file_writable_small.xpm"
#include "jcc_source_file_small.xpm"
#include "jcc_source_file_writable_small.xpm"
#include "jcc_library_file_small.xpm"

#include "jcc_project_file.xpm"
#include "jcc_active_project_file.xpm"
#include "jcc_active_list_file.xpm"

void
CBCreateIcons()
{
	JXImageCache* c = theApplication->GetDisplay(1)->GetImageCache();

	theFileIcon           = c->GetImage(jcc_plain_file_small);
	theWritableFileIcon   = c->GetImage(jcc_plain_file_writable_small);
	theSourceIcon         = c->GetImage(jcc_source_file_small);
	theWritableSourceIcon = c->GetImage(jcc_source_file_writable_small);
	theLibraryIcon        = c->GetImage(jcc_library_file_small);

	theProjectIcon        = c->GetImage(jcc_project_file);
	theActiveProjectIcon  = c->GetImage(jcc_active_project_file);

	theActiveListIcon     = c->GetImage(jcc_active_list_file);
}

void
CBDeleteIcons()
{
}

const JXImage*
CBGetPlainFileIcon()
{
	return theFileIcon;
}

const JXImage*
CBGetWritablePlainFileIcon()
{
	return theWritableFileIcon;
}

const JXImage*
CBGetSourceFileIcon()
{
	return theSourceIcon;
}

const JXImage*
CBGetWritableSourceFileIcon()
{
	return theWritableSourceIcon;
}

const JXImage*
CBGetLibraryFileIcon()
{
	return theLibraryIcon;
}

const JXImage*
CBGetProjectFileIcon
	(
	const JBoolean active
	)
{
	return (active ? theActiveProjectIcon : theProjectIcon);
}

const JXImage*
CBGetTextFileIcon
	(
	const JBoolean active
	)
{
	return (active ? theActiveListIcon : theDocManager->GetDefaultMenuIcon());
}
