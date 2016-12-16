/******************************************************************************
 jXGlobals.cpp

	Access to JX global objects and factories.

	Copyright (C) 1997-99 John Lindal. All rights reserved.

 ******************************************************************************/

#include <jXGlobals.h>
#include <JXUserNotification.h>
#include <JXChooseSaveFile.h>
#include <JXCreatePG.h>
#include <JXSearchTextDialog.h>
#include <JXSpellChecker.h>
#include <JXGetCurrFontMgr.h>
#include <JXGetCurrColormap.h>
#include <JXHelpManager.h>
#include <JXDocumentManager.h>
#include <JXDockManager.h>
#include <JXMDIServer.h>
#include <JXWebBrowser.h>
#include <JXDisplay.h>
#include <JXAssert.h>
#include <JXComposeRuleList.h>
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

static const JCharacter* kInvisibleWindowClass = "Do_not_display_in_taskbar";
static const JCharacter* kDockWindowClass      = "JX_Dock";

#ifdef _J_OSX
static const JCharacter* kX11LocalePath     = "/usr/X11/share/X11/locale/";
static const JCharacter* kDefaultFontName   = "Arial";
#else
static const JCharacter* kX11LocalePath     = "/usr/share/X11/locale/";
static const JCharacter* kDefaultFontName   = "Helvetica";
#endif
static const JCharacter* kMonospaceFontName = "Bitstream Vera Sans Mono";
static const JCharacter* kGreekFontName     = "Symbol";

// Prototypes

void		JXInitLocale();
JBoolean	JXOpenLocaleFile(const JCharacter* fileName, std::ifstream& input);

/******************************************************************************
 JXCreateGlobals

 ******************************************************************************/

void
JXCreateGlobals
	(
	JXApplication*		app,
	const JCharacter*	appSignature,
	const JCharacter**	defaultStringData
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
			  jnew JXGetCurrFontMgr, jnew JXGetCurrColormap,
			  kDefaultFontName, kGreekFontName, kMonospaceFontName);

	XSetErrorHandler(JXDisplay::JXErrorHandler);
	XSetIOErrorHandler(JXApplication::JXIOErrorHandler);

	JXInitLocale();

	// create last so it can access as much as possible

	theSharedPrefsMgr = jnew JXSharedPrefsManager();
	assert( theSharedPrefsMgr != NULL );

	theWebBrowser = jnew JXWebBrowser;
	assert( theWebBrowser != NULL );
	JSetWebBrowser(theWebBrowser);
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

	if (theHelpManager != NULL)
		{
		const JBoolean ok = theHelpManager->Close();
		assert( ok );

		theHelpManager = NULL;
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
}

/******************************************************************************
 JXDeleteGlobals2

	Called after deleting idle and urgent tasks.

 ******************************************************************************/

void
JXDeleteGlobals2()
{
	jdelete theComposeRuleList;
	theComposeRuleList = NULL;

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

	Creates JXHelpManager.  Refer to the JXHelpManager documentation
	for RegisterSection() for information about the arguments to this
	function.

	tocSectionName can be NULL.

 ******************************************************************************/

void
JXInitHelp
	(
	const JCharacter*	tocSectionName,
	const JSize			sectionCount,
	const JCharacter*	sectionName[]
	)
{
	assert( theHelpManager == NULL );
	assert( sectionCount > 0 );

	theHelpManager = jnew JXHelpManager;
	assert( theHelpManager != NULL );

	theHelpManager->SetTOCSectionName(tocSectionName);

	for (JIndex i=0; i<sectionCount; i++)
		{
		theHelpManager->RegisterSection(sectionName[i]);
		}
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

	This version creates the object if it doesn't already exist.

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
	const JCharacter*	dirName,
	JString*			sysDir,
	JString*			userDir
	)
{
	return JGetDataDirectories((JXGetApplication())->GetSignature(),
							   dirName, sysDir, userDir);
}

/******************************************************************************
 JXGetInvisibleWindowClass

 ******************************************************************************/

const JCharacter*
JXGetInvisibleWindowClass()
{
	return kInvisibleWindowClass;
}

/******************************************************************************
 JXGetDockWindowClass

 ******************************************************************************/

const JCharacter*
JXGetDockWindowClass()
{
	return kDockWindowClass;
}

/******************************************************************************
 JXInitLocale (private)

 ******************************************************************************/

const JSize kCharacterCount = 256;
static JBoolean kIsCharacterInWord [ kCharacterCount ];

static JBoolean I18NIsCharacterInWord(const JCharacter c);

void
JXInitLocale()
{
	// IsCharacterInWord

	for (JIndex i=0; i<kCharacterCount; i++)
		{
		kIsCharacterInWord[i] = kJFalse;
		}

	JTextEditor::SetI18NCharacterInWordFunction(I18NIsCharacterInWord);

	// get language name

	const JCharacter* langAlias = getenv("LC_ALL");
	if (langAlias == NULL)
		{
		langAlias = getenv("LANG");
		if (langAlias == NULL)
			{
			langAlias = "POSIX";
			}
		}


	// resolve alias to complete language name

	std::ifstream langInput;
	if (!JXOpenLocaleFile("locale.alias", langInput))
		{
		return;
		}

	JBoolean found = kJFalse;
	JString alias, langName;
	while (!langInput.eof() && !langInput.fail())
		{
		if (langInput.peek() == '#')
			{
			JIgnoreLine(langInput);
			}
		else
			{
			langInput >> std::ws;
			alias    = JReadUntilws(langInput);
			langName = JReadUntilws(langInput);

			if (alias == langAlias)
				{
				found = kJTrue;
				break;
				}
			}
		}
	langInput.close();

	if (!found)
		{
		langName = langAlias;
		}

	// extract file name from Compose.dir

	std::ifstream composeDirInput;
	if (!JXOpenLocaleFile("compose.dir", composeDirInput))
		{
		return;
		}

	found = kJFalse;
	JString composeFile, name;
	while (!composeDirInput.eof() && !composeDirInput.fail())
		{
		if (composeDirInput.peek() == '#')
			{
			JIgnoreLine(composeDirInput);
			}
		else
			{
			composeDirInput >> std::ws;
			composeFile = JReadUntilws(composeDirInput);
			name        = JReadUntilws(composeDirInput);

			if (JString::Compare(name, langName, kJFalse) == 0)
				{
				found = kJTrue;
				break;
				}
			}
		}
	composeDirInput.close();

	// build Compose rule list

	std::ifstream composeInput;
	if (found && JXOpenLocaleFile(composeFile, composeInput))
		{
		theComposeRuleList = jnew JXComposeRuleList(composeInput, kCharacterCount,
													kIsCharacterInWord);
		assert( theComposeRuleList != NULL );
		}
	composeInput.close();
}

/******************************************************************************
 JXOpenLocaleFile (private)

	Until we know that X11R5 used the same file formats, we shouldn't try
	to support R5 at all.

 ******************************************************************************/

JBoolean
JXOpenLocaleFile
	(
	const JCharacter*	fileName,
	std::ifstream&			input
	)
{
	input.close();

	input.clear();
	JString fullName = JCombinePathAndName(kX11LocalePath, fileName);
	input.open(fullName);
	if (input.good())
		{
		return kJTrue;
		}

	return kJFalse;
}

/******************************************************************************
 JXGetComposeRuleList

 ******************************************************************************/

JBoolean
JXGetComposeRuleList
	(
	JXComposeRuleList** ruleList
	)
{
	*ruleList = theComposeRuleList;
	return JI2B( theComposeRuleList != NULL );
}

/******************************************************************************
 JXPreprocessKeyPress

	Returns kJTrue if the KeyPress event should be processed normally.

 ******************************************************************************/

JBoolean
JXPreprocessKeyPress
	(
	const KeySym	keySym,
	JCharacter*		buffer
	)
{
	if (theComposeRuleList != NULL)
		{
		return theComposeRuleList->HandleKeyPress(keySym, buffer);
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 JXPreprocessKeyPress

 ******************************************************************************/

void
JXClearKeyPressPreprocessor()
{
	if (theComposeRuleList != NULL)
		{
		theComposeRuleList->ClearState();
		}
}

/******************************************************************************
 I18NIsCharacterInWord (static)

	1:
		return JI2B('\xC0' <= c && c <= '\xFF' &&
					c != '\xD7' && c != '\xF7');

	2:
	241
	243
	245 - 246
	251 - 254
	256 - 257
	261 - 267
	271 - 274
	276 - 326
	330 - 376

 ******************************************************************************/

JBoolean
I18NIsCharacterInWord
	(
	const JCharacter c
	)
{
	return kIsCharacterInWord [ (unsigned char) c ];
}
