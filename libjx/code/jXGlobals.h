/******************************************************************************
 jXGlobals.h

	Copyright © 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jXGlobals
#define _H_jXGlobals

// we include these for convenience

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jGlobals.h>
#include <jXConstants.h>
#include <JXApplication.h>

class JXDocumentManager;
class JXHelpManager;
class JXDockManager;
class JXMDIServer;
class JXWebBrowser;
class JXChooseSaveFile;
class JXSearchTextDialog;
class JXSpellChecker;
class JXAssert;
class JXComposeRuleList;
class JXSharedPrefsManager;

JXApplication*			JXGetApplication();
JXChooseSaveFile*		JXGetChooseSaveFile();
JXAssert*				JXGetAssertHandler();
JBoolean				JXGetComposeRuleList(JXComposeRuleList** ruleList);
JXSharedPrefsManager*	JXGetSharedPrefsManager();

JXDocumentManager*		JXGetDocumentManager();		// asserts if doesn't exist
JXHelpManager*			JXGetHelpManager();			// asserts if doesn't exist
JXDockManager*			JXGetDockManager();
JBoolean				JXGetDockManager(JXDockManager** dockMgr);
JBoolean				JXGetMDIServer(JXMDIServer** server);
JXWebBrowser*			JXGetWebBrowser();
JXSpellChecker*			JXGetSpellChecker();

JXDirector*				JXGetPersistentWindowOwner();
JXSearchTextDialog*		JXGetSearchTextDialog();

JBoolean				JXGetProgramDataDirectories(const JCharacter* dirName,
													JString* sysDir, JString* userDir);

const JCharacter*		JXGetInvisibleWindowClass();
const JCharacter*		JXGetDockWindowClass();

	// called by main() or derived classes of JXApplication

void	JXCreateDefaultDocumentManager(const JBoolean wantShortcuts = kJTrue);

void	JXInitHelp(const JCharacter* tocSectionName,
				   const JSize sectionCount, const JCharacter* sectionName[]);

	// called by object constructors

void	JXSetDocumentManager(JXDocumentManager* docManager);
void	JXSetMDIServer(JXMDIServer* server);
void	JXSetSearchTextDialog(JXSearchTextDialog* dlog);

	// called by JXApplication

void JXCreateGlobals(JXApplication* app, const JCharacter* appSignature,
					 const JCharacter** defaultStringData);
void JXCloseDirectors();
void JXDeleteGlobals1();
void JXDeleteGlobals2();

void JXSuspendPersistentWindows();
void JXResumePersistentWindows();

	// called by JX objects

JBoolean	JXGetApplication(JXApplication** app);
JBoolean	JXGetDocumentManager(JXDocumentManager** docManager);
JBoolean	JXGetHelpManager(JXHelpManager** helpMgr);
JBoolean	JXGetSearchTextDialog(JXSearchTextDialog** dlog);
void		JXSetDockManager(JXDockManager* dockMgr);

	// called by JXWindow

JBoolean	JXPreprocessKeyPress(const KeySym keySym, JCharacter* buffer);
void		JXClearKeyPressPreprocessor();

	// called by JXFontManager

JBoolean	JXGetCharacterSetName(JString* charSetName);
JBoolean	JXGetLatinCharacterSetIndex(JIndex* charSetIndex);

#endif
