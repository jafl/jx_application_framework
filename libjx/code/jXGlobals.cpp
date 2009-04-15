/******************************************************************************
 jXGlobals.cpp

	Access to JX global objects and factories.

	Copyright © 1997-99 John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
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
#include <locale.h>
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
static JXWebBrowser*			theWebBrowser     = NULL;	// NULL until first needed

static JXDirector*				thePersistentWindowOwner  = NULL;	// can be NULL
static JXSearchTextDialog*		theSearchTextDialog       = NULL;	// can be NULL
static JXSpellChecker*			theSpellChecker           = NULL;

static JXComposeRuleList*		theComposeRuleList        = NULL;	// can be NULL
static JString					theCharacterSetName;
static JIndex					theLatinCharacterSetIndex = 0;

static const JCharacter* kInvisibleWindowClass = "Do_not_display_in_taskbar";
static const JCharacter* kDockWindowClass      = "JX_Dock";

// Prototypes

void		JXInitLocale();
JBoolean	JXOpenLocaleFile(const JCharacter* fileName, ifstream& input);

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

	theAssertHandler = new JXAssert;
	assert( theAssertHandler != NULL );

	JXUserNotification* un = new JXUserNotification;
	assert( un != NULL );

	theChooseSaveFile = new JXChooseSaveFile;
	assert( theChooseSaveFile != NULL );

	JInitCore(theAssertHandler, appSignature, defaultStringData,
			  un, theChooseSaveFile, new JXCreatePG,
			  new JXGetCurrFontMgr, new JXGetCurrColormap,
			  "Helvetica", "Symbol", "Courier");

	XSetErrorHandler(JXDisplay::JXErrorHandler);
	XSetIOErrorHandler(JXApplication::JXIOErrorHandler);

	JXInitLocale();

	// create last so it can access as much as possible

	theSharedPrefsMgr = new JXSharedPrefsManager();
	assert( theSharedPrefsMgr != NULL );
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
	delete theDocManager;
	theDocManager = NULL;

	delete theMDIServer;
	theMDIServer = NULL;

	delete theWebBrowser;
	theWebBrowser = NULL;

	delete theSpellChecker;
	theSpellChecker = NULL;
}

/******************************************************************************
 JXDeleteGlobals2

	Called after deleting idle and urgent tasks.

 ******************************************************************************/

void
JXDeleteGlobals2()
{
	delete theComposeRuleList;
	theComposeRuleList = NULL;

	delete theSharedPrefsMgr;
	theSharedPrefsMgr = NULL;

	theChooseSaveFile = NULL;
	theAssertHandler  = NULL;
	theApplication    = NULL;

	JDeleteGlobals();
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
	JXDocumentManager* docManager = new JXDocumentManager(wantShortcuts);
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

	theHelpManager = new JXHelpManager;
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
	if (theWebBrowser == NULL)
		{
		theWebBrowser = new JXWebBrowser;
		assert( theWebBrowser != NULL );
		}

	return theWebBrowser;
}

/******************************************************************************
 JXGetWebBrowser

	This version returns kJFalse if the object has not been created.

 ******************************************************************************/

JBoolean
JXGetWebBrowser
	(
	JXWebBrowser** webBrowser
	)
{
	*webBrowser = theWebBrowser;
	return JI2B( theWebBrowser != NULL );
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
		thePersistentWindowOwner = new JXDirector(NULL);
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
		theSpellChecker = new JXSpellChecker();
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
 JXGetHelveticaFontName

 ******************************************************************************/

const JCharacter*
JXGetHelveticaFontName()
{
	return "Helvetica";
}

/******************************************************************************
 JXGetTimesFontName

 ******************************************************************************/

const JCharacter*
JXGetTimesFontName()
{
	return "Times";
}

/******************************************************************************
 JXGetCourierFontName

 ******************************************************************************/

const JCharacter*
JXGetCourierFontName()
{
	return "Courier";
}

/******************************************************************************
 JXGetSymbolFontName

 ******************************************************************************/

const JCharacter*
JXGetSymbolFontName()
{
	return "Symbol";
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

static const JCharacter* kISO8859Prefix = "ISO8859-";
const JSize kISO8859PrefixLength        = strlen(kISO8859Prefix);

static const JCharacter* kUTF8Suffix = ".UTF-8";
const JSize kUTF8SuffixLength        = strlen(kUTF8Suffix);

const JSize kCharacterCount = 256;
static JBoolean kIsCharacterInWord [ kCharacterCount ];
static JCharacter kDiacriticalMap  [ kCharacterCount ];
static JIndex kDiacriticalMarkType [ kCharacterCount ];

static JBoolean I18NIsCharacterInWord(const JCharacter c);

void
JXInitLocale()
{
	// IsCharacterInWord

	for (JIndex i=0; i<kCharacterCount; i++)
		{
		kIsCharacterInWord[i]   = kJFalse;
		kDiacriticalMap[i]      = static_cast<JCharacter>(i);
		kDiacriticalMarkType[i] = 0;
		}

	JTextEditor::SetI18NCharacterInWordFunction(I18NIsCharacterInWord);

	// get language name

	const JCharacter* langAliasPtr = getenv("LC_ALL");
	if (langAliasPtr == NULL)
		{
		langAliasPtr = getenv("LANG");
		if (langAliasPtr == NULL)
			{
			langAliasPtr = "POSIX";
			}
		}

	// We don't understand UTF-8, so ask for the plain language instead

	JString langAlias = langAliasPtr;
	if (langAlias.EndsWith(kUTF8Suffix))
		{
		langAlias.RemoveSubstring(langAlias.GetLength() - kUTF8SuffixLength + 1, langAlias.GetLength());
		setenv("LC_ALL", langAlias, kJTrue);
		}

	setlocale(LC_ALL, "");	// for Xkb support (e.g. Russian)

	// resolve alias to complete language name

	ifstream langInput;
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
			langInput >> ws;
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

	// extract Latin charset index

	JIndex langIndex;
	if (langName.LocateLastSubstring(".", &langIndex))
		{
		theCharacterSetName = langName.GetSubstring(langIndex+1, langName.GetLength());
		}
	else
		{
		theCharacterSetName.Clear();
		}

	JIndex isoIndex;
	if (langName.LocateLastSubstring(kISO8859Prefix, kISO8859PrefixLength, kJFalse, &isoIndex) &&
		isoIndex + kISO8859PrefixLength <= langName.GetLength())
		{
		isoIndex             += kISO8859PrefixLength;
		const JString csiStr = langName.GetSubstring(isoIndex, langName.GetLength());
		if (!csiStr.ConvertToUInt(&theLatinCharacterSetIndex) ||
			theLatinCharacterSetIndex == 0)
			{
			theLatinCharacterSetIndex = 0;
			return;
			}
		}
	else if (langName == "C")
		{
		theLatinCharacterSetIndex = 1;
		}
	else
		{
		theLatinCharacterSetIndex = 0;
		return;
		}

	// extract file name from Compose.dir

	ifstream composeDirInput;
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
			composeDirInput >> ws;
			composeFile = JReadUntilws(composeDirInput);
			name        = JReadUntilws(composeDirInput);

			if (JStringCompare(name, langName, kJFalse) == 0)
				{
				found = kJTrue;
				break;
				}
			}
		}
	composeDirInput.close();

	// build Compose rule list

	ifstream composeInput;
	if (found && JXOpenLocaleFile(composeFile, composeInput))
		{
		theComposeRuleList = new JXComposeRuleList(composeInput, kCharacterCount,
												   kIsCharacterInWord,
												   kDiacriticalMap,
												   kDiacriticalMarkType);
		assert( theComposeRuleList != NULL );

		JSetDiacriticalMap(kDiacriticalMap, kDiacriticalMarkType);
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
	ifstream&			input
	)
{
	input.close();

	input.clear();
	JString fullName = JCombinePathAndName("/usr/X11R6/lib/X11/locale/", fileName);
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
 JXGetLatinCharacterSetIndex

 ******************************************************************************/

JBoolean
JXGetLatinCharacterSetIndex
	(
	JIndex* charSetIndex
	)
{
	*charSetIndex = theLatinCharacterSetIndex;
	return JI2B( theLatinCharacterSetIndex > 0 );
}

/******************************************************************************
 JXGetCharacterSetName

 ******************************************************************************/

JBoolean
JXGetCharacterSetName
	(
	JString* charSetName
	)
{
	*charSetName = theCharacterSetName;
	return !theCharacterSetName.IsEmpty();
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
