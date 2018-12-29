/******************************************************************************
 SyGGlobals.h

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_SyGGlobals
#define _H_SyGGlobals

// we include these for convenience

#include <jXGlobals.h>
#include <JXDocumentManager.h>
#include <jMountUtil.h>
#include <JXPM.h>
#include "SyGApplication.h"

class JXContainer;
class JXWidget;
class SyGPrefsMgr;
class SyGMDIServer;
class SyGViewManPageDialog;
class SyGFindFileDialog;
class SyGChooseSaveFile;
class SyGFileTreeTable;
class SyGFileTreeList;

SyGApplication*			SyGGetApplication();
SyGPrefsMgr*			SyGGetPrefsMgr();
SyGMDIServer*			SyGGetMDIServer();
SyGViewManPageDialog*	SyGGetManPageDialog();
SyGFindFileDialog*		SyGGetFindFileDialog();
SyGChooseSaveFile*		SyGGetChooseSaveFile();

JBoolean	SyGIsTrashDirectory(const JString& path);
JBoolean	SyGGetTrashDirectory(JString* path, const JBoolean reportErrors = kJTrue);
JBoolean	SyGTrashDirectoryIsEmpty();
void		SyGUpdateTrash();
JBoolean	SyGEmptyTrashDirectory();

JBoolean	SyGDeleteDirEntry(const JString& fullName);
JBoolean	SyGExec(const JString& cmd, const JBoolean report = kJTrue);

	// called by SyGApplication

JBoolean	SyGCreateGlobals(SyGApplication* app);
void		SyGDeleteGlobals();
void		SyGCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by SyGRecentFileMenu

JBoolean	SyGGetRecentFileDirectory(JString* path, const JBoolean reportErrors = kJTrue);
void		SyGAddRecentFile(const JString& fullname);

	// used during intra-app DND

JBoolean	SyGGetDNDSource(const JXWidget* dndSource, SyGFileTreeTable** widget);
void		SyGSetDNDSource(SyGFileTreeTable* widget);

JBoolean	SyGGetDNDTarget(const JXContainer* dndTarget, SyGFileTreeTable** widget);
void		SyGSetDNDTarget(SyGFileTreeTable* widget);

void		SyGGetDNDAskActions(JArray<Atom>* actionList,
								JPtrArray<JString>* descriptionList);

	// icons

JXImage*	SyGGetFileSmallIcon(const JBoolean selected = kJFalse);
JXImage*	SyGGetFolderSmallIcon(const JBoolean selected = kJFalse);
JXImage*	SyGGetReadOnlyFolderSmallIcon(const JBoolean selected = kJFalse);
JXImage*	SyGGetLockedFolderSmallIcon(const JBoolean selected = kJFalse);
JXImage*	SyGGetExecSmallIcon(const JBoolean selected = kJFalse);
JXImage*	SyGGetUnknownSmallIcon(const JBoolean selected = kJFalse);
JXImage*	SyGGetDirectorySmallIcon(const JString& path);
JIndex		SyGGetMountPointLargeIcon(const JString& path, SyGFileTreeList* fileList,
									  JXPM* plainIcon, JXPM* selectedIcon);

JBoolean	SyGGetMountPointSmallIcon(const JMountType type, JXImage** image);
JXImage*	SyGGetTrashSmallIcon(const JBoolean selected = kJFalse);

	// version info

const JString&	SyGGetVersionNumberStr();
JString			SyGGetVersionStr();

	// called by Directors

const JUtf8Byte*	SyGGetWMClassInstance();
const JUtf8Byte*	SyGGetFolderWindowClass();

#endif
