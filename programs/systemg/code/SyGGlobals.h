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

bool	SyGIsTrashDirectory(const JString& path);
bool	SyGGetTrashDirectory(JString* path, const bool reportErrors = true);
bool	SyGTrashDirectoryIsEmpty();
void		SyGUpdateTrash();
bool	SyGEmptyTrashDirectory();

bool	SyGDeleteDirEntry(const JString& fullName);
bool	SyGExec(const JString& cmd, const bool report = true);

	// called by SyGApplication

bool	SyGCreateGlobals(SyGApplication* app);
void		SyGDeleteGlobals();
void		SyGCleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);

	// called by SyGRecentFileMenu

bool	SyGGetRecentFileDirectory(JString* path, const bool reportErrors = true);
void		SyGAddRecentFile(const JString& fullname);

	// used during intra-app DND

bool	SyGGetDNDSource(const JXWidget* dndSource, SyGFileTreeTable** widget);
void		SyGSetDNDSource(SyGFileTreeTable* widget);

bool	SyGGetDNDTarget(const JXContainer* dndTarget, SyGFileTreeTable** widget);
void		SyGSetDNDTarget(SyGFileTreeTable* widget);

void		SyGGetDNDAskActions(JArray<Atom>* actionList,
								JPtrArray<JString>* descriptionList);

	// icons

JXImage*	SyGGetFileSmallIcon(const bool selected = false);
JXImage*	SyGGetFolderSmallIcon(const bool selected = false);
JXImage*	SyGGetReadOnlyFolderSmallIcon(const bool selected = false);
JXImage*	SyGGetLockedFolderSmallIcon(const bool selected = false);
JXImage*	SyGGetExecSmallIcon(const bool selected = false);
JXImage*	SyGGetUnknownSmallIcon(const bool selected = false);
JXImage*	SyGGetDirectorySmallIcon(const JString& path);
JIndex		SyGGetMountPointLargeIcon(const JString& path, SyGFileTreeList* fileList,
									  JXPM* plainIcon, JXPM* selectedIcon);

bool	SyGGetMountPointSmallIcon(const JMountType type, JXImage** image);
JXImage*	SyGGetTrashSmallIcon(const bool selected = false);

	// version info

const JString&	SyGGetVersionNumberStr();
JString			SyGGetVersionStr();

	// called by Directors

const JUtf8Byte*	SyGGetWMClassInstance();
const JUtf8Byte*	SyGGetFolderWindowClass();

#endif
