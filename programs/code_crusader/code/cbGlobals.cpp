/******************************************************************************
 cbGlobals.cpp

	Access to global objects and factories.

	Copyright © 1997-99 John Lindal.

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

static CBApp*				theApplication       = nullptr;	// owns itself
static CBDocumentManager*	theDocManager        = nullptr;	// owned by JX
static CBPrefsManager*		thePrefsManager      = nullptr;
static CBMDIServer*			theMDIServer         = nullptr;	// owned by JX

static CBSearchTextDialog*	theSearchTextDialog  = nullptr;	// owned by JXGetPersistentWindowOwner()
static CBRunTEScriptDialog*	theRunTEScriptDialog = nullptr;	// owned by JXGetPersistentWindowOwner()
static CBViewManPageDialog*	theViewManPageDialog = nullptr;	// owned by JXGetPersistentWindowOwner()
static CBFindFileDialog*	theFindFileDialog    = nullptr;	// owned by JXGetPersistentWindowOwner()
static CBDiffFileDialog*	theDiffFileDialog    = nullptr;	// owned by JXGetPersistentWindowOwner()

static CBCommandManager*	theCmdManager        = nullptr;
static CBFnMenuUpdater*		theFnMenuUpdater     = nullptr;
static CBSymbolTypeList*	theSymbolTypeList    = nullptr;

static CBPTPrinter*			thePTTextPrinter     = nullptr;
static CBPSPrinter*			thePSTextPrinter     = nullptr;

static const JUtf8Byte* kProjectWindowClass       = "Code_Crusader_Project";
static const JUtf8Byte* kSymbolWindowClass        = "Code_Crusader_Symbol_List";
static const JUtf8Byte* kTreeWindowClass          = "Code_Crusader_Tree";
static const JUtf8Byte* kFileListWindowClass      = "Code_Crusader_File_List";
static const JUtf8Byte* kEditorWindowClass        = "Code_Crusader_Editor";
static const JUtf8Byte* kExecOutputWindowClass    = "Code_Crusader_Editor_Exec_Output";
static const JUtf8Byte* kCompileOutputWindowClass = "Code_Crusader_Editor_Compile_Output";
static const JUtf8Byte* kSearchOutputWindowClass  = "Code_Crusader_Editor_Search_Output";
static const JUtf8Byte* kShellWindowClass         = "Code_Crusader_Editor_Shell";

// owned by JXImageCache
static JXImage* theFileIcon           = nullptr;
static JXImage* theWritableFileIcon   = nullptr;
static JXImage* theSourceIcon         = nullptr;
static JXImage* theWritableSourceIcon = nullptr;
static JXImage* theLibraryIcon        = nullptr;
static JXImage* theProjectIcon        = nullptr;
static JXImage* theActiveProjectIcon  = nullptr;
static JXImage* theActiveListIcon     = nullptr;

static bool theIsUpdateThread = false;

// private functions

void	CBCreateIcons();
void	CBDeleteIcons();

/******************************************************************************
 CBCreateGlobals

	Returns true if this is the first time the program is run.

 ******************************************************************************/

bool
CBCreateGlobals
	(
	CBApp*			app,
	const bool	useMDI
	)
{
	if (JAssertBase::GetAction() == JAssertBase::kAskUser)
		{
		JAssertBase::SetAction(JAssertBase::kAbort);
		}

	theApplication = app;

	CBCreateIcons();

	// prefs must be created first so everybody can read from it

	bool isNew;
	thePrefsManager = jnew CBPrefsManager(&isNew);
	assert( thePrefsManager != nullptr );

	JXInitHelp();

	theDocManager = jnew CBDocumentManager;
	assert( theDocManager != nullptr );

	if (useMDI)
		{
		auto* dockManager = jnew CBDockManager();
		assert( dockManager != nullptr );
		dockManager->JPrefObject::ReadPrefs();

		theMDIServer = jnew CBMDIServer;
		assert( theMDIServer != nullptr );
		}

	theSearchTextDialog = CBSearchTextDialog::Create();
	assert( theSearchTextDialog != nullptr );

	theRunTEScriptDialog = jnew CBRunTEScriptDialog(JXGetPersistentWindowOwner());
	assert( theRunTEScriptDialog != nullptr );

	theViewManPageDialog = jnew CBViewManPageDialog(JXGetPersistentWindowOwner());
	assert( theViewManPageDialog != nullptr );

	theFindFileDialog = jnew CBFindFileDialog(JXGetPersistentWindowOwner());
	assert( theFindFileDialog != nullptr );

	theDiffFileDialog = jnew CBDiffFileDialog(JXGetPersistentWindowOwner());
	assert( theDiffFileDialog != nullptr );

	// widgets hidden in permanent window

	JXWindow* permWindow = theSearchTextDialog->GetWindow();

	theDocManager->CreateFileHistoryMenus(permWindow);

	// global commands

	theCmdManager = jnew CBCommandManager(theDocManager);
	assert( theCmdManager != nullptr );
	theCmdManager->JPrefObject::ReadPrefs();

	// fn menu updater

	theFnMenuUpdater = jnew CBFnMenuUpdater;
	assert( theFnMenuUpdater != nullptr );

	// symbol type list

	theSymbolTypeList = jnew CBSymbolTypeList(permWindow->GetDisplay());
	assert( theSymbolTypeList != nullptr );

	// printers

	thePTTextPrinter = jnew CBPTPrinter;
	assert( thePTTextPrinter != nullptr );

	thePSTextPrinter = jnew CBPSPrinter(permWindow->GetDisplay());
	assert( thePSTextPrinter != nullptr );

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

	if (theMDIServer != nullptr)
		{
		theMDIServer->JPrefObject::WritePrefs();
		}

	CBDeleteIcons();

	theApplication = nullptr;
	theDocManager  = nullptr;
	theMDIServer   = nullptr;

	theCmdManager->JPrefObject::WritePrefs();
	jdelete theCmdManager;
	theCmdManager = nullptr;

	jdelete theFnMenuUpdater;
	theFnMenuUpdater = nullptr;

	jdelete theSymbolTypeList;
	theSymbolTypeList = nullptr;

	// printers

	jdelete thePTTextPrinter;
	thePTTextPrinter = nullptr;

	jdelete thePSTextPrinter;
	thePSTextPrinter = nullptr;

	CBShutdownCompleters();
	CBShutdownStylers();

	// windows closed by JXGetPersistentWindowOwner()

	theSearchTextDialog->JPrefObject::WritePrefs();
	theRunTEScriptDialog->JPrefObject::WritePrefs();
	theViewManPageDialog->JPrefObject::WritePrefs();
	theFindFileDialog->JPrefObject::WritePrefs();
	theDiffFileDialog->JPrefObject::WritePrefs();

	theSearchTextDialog  = nullptr;
	theRunTEScriptDialog = nullptr;
	theViewManPageDialog = nullptr;
	theFindFileDialog    = nullptr;
	theDiffFileDialog    = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsManager;
	thePrefsManager = nullptr;
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

		if (theMDIServer != nullptr)
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
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 CBInUpdateThread

 ******************************************************************************/

bool
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
	theIsUpdateThread = true;
}

/******************************************************************************
 CBGetDocumentManager

 ******************************************************************************/

CBDocumentManager*
CBGetDocumentManager()
{
	assert( theDocManager != nullptr );
	return theDocManager;
}

/******************************************************************************
 CBGetPrefsManager

 ******************************************************************************/

CBPrefsManager*
CBGetPrefsManager()
{
	assert( thePrefsManager != nullptr );
	return thePrefsManager;
}

/******************************************************************************
 CBGetMDIServer

 ******************************************************************************/

bool
CBGetMDIServer
	(
	CBMDIServer** mdi
	)
{
	*mdi = theMDIServer;
	return theMDIServer != nullptr;
}

/******************************************************************************
 CBGetSearchTextDialog

 ******************************************************************************/

CBSearchTextDialog*
CBGetSearchTextDialog()
{
	assert( theSearchTextDialog != nullptr );
	return theSearchTextDialog;
}

/******************************************************************************
 CBGetRunTEScriptDialog

 ******************************************************************************/

CBRunTEScriptDialog*
CBGetRunTEScriptDialog()
{
	assert( theRunTEScriptDialog != nullptr );
	return theRunTEScriptDialog;
}

/******************************************************************************
 CBGetViewManPageDialog

 ******************************************************************************/

CBViewManPageDialog*
CBGetViewManPageDialog()
{
	assert( theViewManPageDialog != nullptr );
	return theViewManPageDialog;
}

/******************************************************************************
 CBGetFindFileDialog

 ******************************************************************************/

CBFindFileDialog*
CBGetFindFileDialog()
{
	assert( theFindFileDialog != nullptr );
	return theFindFileDialog;
}

/******************************************************************************
 CBGetDiffFileDialog

 ******************************************************************************/

CBDiffFileDialog*
CBGetDiffFileDialog()
{
	assert( theDiffFileDialog != nullptr );
	return theDiffFileDialog;
}

/******************************************************************************
 CBGetCommandManager

 ******************************************************************************/

CBCommandManager*
CBGetCommandManager()
{
	assert( theCmdManager != nullptr );
	return theCmdManager;
}

/******************************************************************************
 CBGetFnMenuUpdater

 ******************************************************************************/

CBFnMenuUpdater*
CBGetFnMenuUpdater()
{
	assert( theFnMenuUpdater != nullptr );
	return theFnMenuUpdater;
}

/******************************************************************************
 CBGetSymbolTypeList

 ******************************************************************************/

CBSymbolTypeList*
CBGetSymbolTypeList()
{
	assert( theSymbolTypeList != nullptr );
	return theSymbolTypeList;
}

/******************************************************************************
 CBGetPTTextPrinter

 ******************************************************************************/

CBPTPrinter*
CBGetPTTextPrinter()
{
	assert( thePTTextPrinter != nullptr );
	return thePTTextPrinter;
}

/******************************************************************************
 CBGetPSTextPrinter

 ******************************************************************************/

CBPSPrinter*
CBGetPSTextPrinter()
{
	assert( thePSTextPrinter != nullptr );
	return thePSTextPrinter;
}

/******************************************************************************
 CBGetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
CBGetWMClassInstance()
{
	return JGetString("CBName").GetBytes();
}

const JUtf8Byte*
CBGetProjectWindowClass()
{
	return kProjectWindowClass;
}

const JUtf8Byte*
CBGetSymbolWindowClass()
{
	return kSymbolWindowClass;
}

const JUtf8Byte*
CBGetTreeWindowClass()
{
	return kTreeWindowClass;
}

const JUtf8Byte*
CBGetFileListWindowClass()
{
	return kFileListWindowClass;
}

const JUtf8Byte*
CBGetEditorWindowClass()
{
	return kEditorWindowClass;
}

const JUtf8Byte*
CBGetExecOutputWindowClass()
{
	return kExecOutputWindowClass;
}

const JUtf8Byte*
CBGetCompileOutputWindowClass()
{
	return kCompileOutputWindowClass;
}

const JUtf8Byte*
CBGetSearchOutputWindowClass()
{
	return kSearchOutputWindowClass;
}

const JUtf8Byte*
CBGetShellWindowClass()
{
	return kShellWindowClass;
}

/******************************************************************************
 CBGetVersionNumberStr

 ******************************************************************************/

const JString&
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
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("Description::cbGlobals", map, sizeof(map));
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
	const bool active
	)
{
	return (active ? theActiveProjectIcon : theProjectIcon);
}

const JXImage*
CBGetTextFileIcon
	(
	const bool active
	)
{
	return (active ? theActiveListIcon : theDocManager->GetDefaultMenuIcon());
}
