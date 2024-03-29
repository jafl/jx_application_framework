/******************************************************************************
 jXGlobals.h

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_jXGlobals
#define _H_jXGlobals

// we include these for convenience

#include <jx-af/jcore/jGlobals.h>
#include "jXConstants.h"
#include "JXApplication.h"

class JXDocumentManager;
class JXHelpManager;
class JXDockManager;
class JXMDIServer;
class JXWebBrowser;
class JXSearchTextDialog;
class JXSpellChecker;
class JXAssert;
class JXComposeRuleList;
class JXSharedPrefsManager;

JXApplication*			JXGetApplication();
JXAssert*				JXGetAssertHandler();
bool					JXGetComposeRuleList(JXComposeRuleList** ruleList);
JXSharedPrefsManager*	JXGetSharedPrefsManager();

JXDocumentManager*		JXGetDocumentManager();		// asserts if doesn't exist
JXHelpManager*			JXGetHelpManager();			// asserts if doesn't exist
JXDockManager*			JXGetDockManager();
bool					JXGetDockManager(JXDockManager** dockMgr);
bool					JXGetMDIServer(JXMDIServer** server);
JXWebBrowser*			JXGetWebBrowser();
JXSpellChecker*			JXGetSpellChecker();

JXDirector*				JXGetPersistentWindowOwner();
JXSearchTextDialog*		JXGetSearchTextDialog();

bool					JXGetProgramDataDirectories(const JUtf8Byte* dirName,
													JString* sysDir, JString* userDir);

const JUtf8Byte*		JXGetInvisibleWindowClass();
const JUtf8Byte*		JXGetDockWindowClass();

	// called by main() or derived classes of JXApplication

void	JXCreateDefaultDocumentManager(const bool wantShortcuts = true);

void	JXInitHelp();

	// called by object constructors

void	JXSetDocumentManager(JXDocumentManager* docManager);
void	JXSetMDIServer(JXMDIServer* server);
void	JXSetSearchTextDialog(JXSearchTextDialog* dlog);

	// called by JXApplication

void JXCreateGlobals(JXApplication* app, const JUtf8Byte* appSignature,
					 const JUtf8Byte** defaultStringData);
void JXCloseDirectors();
void JXDeleteGlobals1();
void JXDeleteGlobals2();

void JXSuspendPersistentWindows();
void JXResumePersistentWindows();

	// called by JX objects

bool	JXGetApplication(JXApplication** app);
bool	JXGetDocumentManager(JXDocumentManager** docManager);
bool	JXGetHelpManager(JXHelpManager** helpMgr);
bool	JXGetSearchTextDialog(JXSearchTextDialog** dlog);
void	JXSetDockManager(JXDockManager* dockMgr);

#endif
