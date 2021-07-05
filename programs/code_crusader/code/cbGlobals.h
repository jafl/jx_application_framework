/******************************************************************************
 cbGlobals.h

	Copyright © 1997-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_cbGlobals
#define _H_cbGlobals

// we include these for convenience

#include <jXGlobals.h>
#include "CBApp.h"
#include "CBDocumentManager.h"
#include "CBPrefsManager.h"

class CBMDIServer;
class CBSearchTextDialog;
class CBRunTEScriptDialog;
class CBViewManPageDialog;
class CBFindFileDialog;
class CBDiffFileDialog;
class CBCommandManager;
class CBFnMenuUpdater;
class CBSymbolTypeList;
class CBPTPrinter;
class CBPSPrinter;

CBApp*					CBGetApplication();
CBDocumentManager*		CBGetDocumentManager();
CBPrefsManager*			CBGetPrefsManager();
JBoolean				CBGetMDIServer(CBMDIServer** mdi);

CBSearchTextDialog*		CBGetSearchTextDialog();
CBRunTEScriptDialog*	CBGetRunTEScriptDialog();
CBViewManPageDialog*	CBGetViewManPageDialog();
CBFindFileDialog*		CBGetFindFileDialog();
CBDiffFileDialog*		CBGetDiffFileDialog();

CBCommandManager*		CBGetCommandManager();
CBFnMenuUpdater*		CBGetFnMenuUpdater();
CBSymbolTypeList*		CBGetSymbolTypeList();

CBPTPrinter*			CBGetPTTextPrinter();
CBPSPrinter*			CBGetPSTextPrinter();

const JString&			CBGetVersionNumberStr();
JString					CBGetVersionStr();

JBoolean				CBInUpdateThread();
void					CBSetUpdateThread();

	// icons

const JXImage*	CBGetPlainFileIcon();
const JXImage*	CBGetWritablePlainFileIcon();
const JXImage*	CBGetSourceFileIcon();
const JXImage*	CBGetWritableSourceFileIcon();
const JXImage*	CBGetLibraryFileIcon();
const JXImage*	CBGetProjectFileIcon(const JBoolean active);
const JXImage*	CBGetTextFileIcon(const JBoolean active);

	// called by CBApp

JBoolean CBCreateGlobals(CBApp* app, const JBoolean useMDI);
void     CBDeleteGlobals();
void     CBCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by Directors

const JUtf8Byte*	CBGetWMClassInstance();
const JUtf8Byte*	CBGetProjectWindowClass();
const JUtf8Byte*	CBGetSymbolWindowClass();
const JUtf8Byte*	CBGetTreeWindowClass();
const JUtf8Byte*	CBGetFileListWindowClass();
const JUtf8Byte*	CBGetEditorWindowClass();
const JUtf8Byte*	CBGetExecOutputWindowClass();
const JUtf8Byte*	CBGetCompileOutputWindowClass();
const JUtf8Byte*	CBGetSearchOutputWindowClass();
const JUtf8Byte*	CBGetShellWindowClass();

#endif
