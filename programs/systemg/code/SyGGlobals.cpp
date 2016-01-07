/******************************************************************************
 SyGGlobals.cpp

	Access to global objects and factories.

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <SyGStdInc.h>
#include "SyGGlobals.h"
#include "SyGPrefsMgr.h"
#include "SyGMDIServer.h"
#include "SyGHelpText.h"
#include "SyGViewManPageDialog.h"
#include "SyGFindFileDialog.h"
#include "SyGChooseSaveFile.h"
#include "SyGFileTreeTable.h"
#include "SyGFileTreeList.h"
#include "SyGFileTree.h"
#include <JXDisplay.h>
#include <JXImageCache.h>
#include <JXImage.h>
#include <JXDNDManager.h>
#include <JXWDManager.h>
#include <JXFSBindingManager.h>
#include <JSimpleProcess.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

static SyGApplication*			theApplication       = NULL;	// owns itself
static SyGPrefsMgr*				thePrefsMgr          = NULL;
static SyGMDIServer*			theMDIServer         = NULL;	// owned by JX

static SyGViewManPageDialog*	theManPageDialog     = NULL;	// not owned
static SyGFindFileDialog*		theFindFileDialog    = NULL;	// not owned
static SyGChooseSaveFile*		theAltChooseSaveFile = NULL;

static SyGFileTreeTable*		theDNDSource         = NULL;	// not owned
static SyGFileTreeTable*		theDNDTarget         = NULL;	// not owned

// owned by JXImageCache
static JXImage*	theFileIcon                   = NULL;
static JXImage*	theSelectedFileIcon           = NULL;
static JXImage*	theFolderIcon                 = NULL;
static JXImage*	theSelectedFolderIcon         = NULL;
static JXImage* theReadOnlyFolderIcon         = NULL;
static JXImage* theSelectedReadOnlyFolderIcon = NULL;
static JXImage* theLockedFolderIcon           = NULL;
static JXImage* theSelectedLockedFolderIcon   = NULL;
static JXImage*	theExecIcon                   = NULL;
static JXImage*	theSelectedExecIcon           = NULL;
static JXImage*	theUnknownIcon                = NULL;
static JXImage*	theSelectedUnknownIcon        = NULL;

static const JCharacter* kTrashDirName      = "/.trashcan/";
static JString theTrashDir;			// only need to compute this once
static JDirInfo* theTrashDirInfo            = NULL;
static const JCharacter* kRecentFileDirName = "/.jxfvwm2taskbar/recent_files/";
static JString theRecentFileDir;	// only need to compute this once
const JSize kRecentFileCount                = 20;

const long kTrashCanPerms      = 0700;
const long kRecentFileDirPerms = 0700;

static const JCharacter* kFolderWindowClass = "SystemG_Folder";

// private functions

void	SyGCreateIcons();
void	SyGDeleteIcons();

// string ID's

static const JCharacter* kDescriptionID              = "Description::SyGGlobals";
static const JCharacter* kCreateTrashErrorID         = "CreateTrashError::SyGGlobals";
static const JCharacter* kEmptyTrashErrorID          = "EmptyTrashError::SyGGlobals";
static const JCharacter* kCreateRecentFileDirErrorID = "CreateRecentFileDirError::SyGGlobals";
static const JCharacter* kNoPrefsDirID               = "NoPrefsDir::SyGGlobals";
static const JCharacter* kDNDCopyDescriptionID       = "DNDCopyDescription::SyGGlobals";
static const JCharacter* kDNDMoveDescriptionID       = "DNDMoveDescription::SyGGlobals";
static const JCharacter* kDNDLinkDescriptionID       = "DNDLinkDescription::SyGGlobals";

/******************************************************************************
 SyGCreateGlobals

 ******************************************************************************/

JBoolean
SyGCreateGlobals
	(
	SyGApplication* app
	)
{
	theApplication = app;

	SyGCreateIcons();

	JString oldPrefsFile, newPrefsFile;
	if (JGetPrefsDirectory(&oldPrefsFile))
		{
		oldPrefsFile = JCombinePathAndName(oldPrefsFile, ".gSystemG.pref");
		if (JFileExists(oldPrefsFile) &&
			(JPrefsFile::GetFullName(app->GetSignature(), &newPrefsFile)).OK() &&
			!JFileExists(newPrefsFile))
			{
			JRenameFile(oldPrefsFile, newPrefsFile);
			}
		}

	JBoolean isNew;
	thePrefsMgr = new SyGPrefsMgr(&isNew);
	assert(thePrefsMgr != NULL);

	JXInitHelp(kSyGTOCHelpName, kSyGHelpSectionCount, kSyGHelpSectionName);

	JXWDManager* wdMgr = new JXWDManager(app->GetCurrentDisplay(), kJTrue);
	assert( wdMgr != NULL );
	// registers itself

	theMDIServer = new SyGMDIServer;
	assert( theMDIServer != NULL );

	theManPageDialog = new SyGViewManPageDialog(JXGetPersistentWindowOwner());
	assert( theManPageDialog != NULL );

	theFindFileDialog = new SyGFindFileDialog(JXGetPersistentWindowOwner());
	assert( theFindFileDialog != NULL );

	theAltChooseSaveFile = new SyGChooseSaveFile(thePrefsMgr, kSAltCSSetupID);
	assert( theAltChooseSaveFile != NULL );

	JString trashDir;
	SyGGetTrashDirectory(&trashDir, kJFalse);	// silently creates it

	return isNew;
}

/******************************************************************************
 SyGDeleteGlobals

 ******************************************************************************/

void
SyGDeleteGlobals()
{
	theMDIServer = NULL;

	// window closed by JXGetPersistentWindowOwner()

	theManPageDialog->JPrefObject::WritePrefs();
	theManPageDialog = NULL;

	theFindFileDialog->JPrefObject::WritePrefs();
	theFindFileDialog = NULL;

	delete theAltChooseSaveFile;
	theAltChooseSaveFile = NULL;

	delete theTrashDirInfo;
	theTrashDirInfo = NULL;

	SyGDeleteIcons();

	JXFSBindingManager::Destroy();

	theApplication = NULL;
	theDNDSource   = NULL;
	theDNDTarget   = NULL;

	// this must be last so everybody else can use it to save their setup

	delete thePrefsMgr;
	thePrefsMgr = NULL;
}

/******************************************************************************
 SyGCleanUpBeforeSuddenDeath

	This must be the last one called by SyGApplication so we can save
	the preferences to disk.

	*** If the server is dead, you cannot call any code that contacts it.

 ******************************************************************************/

void
SyGCleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	if (reason != JXDocumentManager::kAssertFired)
		{
		theAltChooseSaveFile->JPrefObject::WritePrefs();
		theManPageDialog->JPrefObject::WritePrefs();
		theFindFileDialog->JPrefObject::WritePrefs();
		}

	// must be last to save everything

	thePrefsMgr->CleanUpBeforeSuddenDeath(reason);
}

/******************************************************************************
 SyGGetApplication

 ******************************************************************************/

SyGApplication*
SyGGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 GGetPrefsMgr

 ******************************************************************************/

SyGPrefsMgr*
SyGGetPrefsMgr()
{
	assert(thePrefsMgr != NULL);
	return thePrefsMgr;
}

/******************************************************************************
 SyGGetMDIServer

 ******************************************************************************/

SyGMDIServer*
SyGGetMDIServer()
{
	assert( theMDIServer != NULL );
	return theMDIServer;
}

/******************************************************************************
 SyGIsTrashDirectory

 ******************************************************************************/

JBoolean
SyGIsTrashDirectory
	(
	const JCharacter* path
	)
{
	JString dir;
	return JI2B(SyGGetTrashDirectory(&dir, kJFalse) &&
				JSameDirEntry(dir, path));
}

/******************************************************************************
 SyGGetTrashDirectory

 ******************************************************************************/

JBoolean
SyGGetTrashDirectory
	(
	JString*		path,
	const JBoolean	reportErrors
	)
{
	if (!theTrashDir.IsEmpty())
		{
		*path = theTrashDir;
		return kJTrue;
		}

	if (!JGetPrefsDirectory(path))
		{
		if (reportErrors)
			{
			(JGetUserNotification())->ReportError(JGetString(kNoPrefsDirID));
			}
		return kJFalse;
		}

	*path = JCombinePathAndName(*path, kTrashDirName);

	JError err = JNoError();
	if (!JDirectoryExists(*path))
		{
		err = JCreateDirectory(*path, kTrashCanPerms);
		}
	else
		{
		err = JSetPermissions(*path, kTrashCanPerms);
		}

	if (err.OK())
		{
		theTrashDir       = *path;
		const JBoolean ok = JDirInfo::Create(theTrashDir, &theTrashDirInfo);
		assert( ok );
		return kJTrue;
		}
	else
		{
		path->Clear();
		if (reportErrors)
			{
			(JGetStringManager())->ReportError(kCreateTrashErrorID, err);
			}
		return kJFalse;
		}
}

/******************************************************************************
 SyGTrashDirectoryIsEmpty

 ******************************************************************************/

JBoolean
SyGTrashDirectoryIsEmpty()
{
	if (theTrashDirInfo != NULL)
		{
		theTrashDirInfo->Update();
		return theTrashDirInfo->IsEmpty();
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 SyGUpdateTrash

 ******************************************************************************/

void
SyGUpdateTrash()
{
	if (theTrashDirInfo != NULL)
		{
		theTrashDirInfo->ForceUpdate();
		(SyGGetApplication())->UpdateTrash();
		}
}

/******************************************************************************
 SyGEmptyTrashDirectory

 ******************************************************************************/

JBoolean
SyGEmptyTrashDirectory()
{
	JString trashDir;
	const JBoolean hasTrash = SyGGetTrashDirectory(&trashDir, kJFalse);
	if (hasTrash && JKillDirectory(trashDir))
		{
		JCreateDirectory(trashDir, kTrashCanPerms);
		SyGUpdateTrash();
		return kJTrue;
		}
	else if (hasTrash)
		{
		(JGetUserNotification())->ReportError(JGetString(kEmptyTrashErrorID));
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}

/******************************************************************************
 SyGDeleteDirEntry

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
SyGDeleteDirEntry
	(
	const JCharacter* fullName
	)
{
	const JString sysCmd = "rm -rf " + JPrepArgForExec(fullName);
	return SyGExec(sysCmd);
}

/******************************************************************************
 SyGExec

	Returns kJTrue if successful.

 ******************************************************************************/

JBoolean
SyGExec
	(
	const JCharacter*	cmd,
	const JBoolean		report
	)
{
	JString errOutput;
	const JError err = JRunProgram(cmd, &errOutput);
	if (!err.OK() && report)
		{
		(JGetUserNotification())->ReportError(err.GetMessage());
		}
	return err.OK();
}

/******************************************************************************
 SyGGetManPageDialog

 ******************************************************************************/

SyGViewManPageDialog*
SyGGetManPageDialog()
{
	assert( theManPageDialog != NULL );
	return theManPageDialog;
}

/******************************************************************************
 SyGGetFindFileDialog

 ******************************************************************************/

SyGFindFileDialog*
SyGGetFindFileDialog()
{
	assert( theFindFileDialog != NULL );
	return theFindFileDialog;
}

/******************************************************************************
 SyGGetChooseSaveFile

 ******************************************************************************/

SyGChooseSaveFile*
SyGGetChooseSaveFile()
{
	assert( theAltChooseSaveFile != NULL );
	return theAltChooseSaveFile;
}

/******************************************************************************
 DND source

	dndSource can be NULL

 ******************************************************************************/

JBoolean
SyGGetDNDSource
	(
	const JXWidget*		dndSource,
	SyGFileTreeTable**	widget
	)
{
	if (dndSource != theDNDSource)
		{
		theDNDSource = NULL;
		}

	*widget = theDNDSource;
	return JI2B( theDNDSource != NULL );
}

void
SyGSetDNDSource
	(
	SyGFileTreeTable* widget
	)
{
	theDNDSource = widget;
}

/******************************************************************************
 DND target

	dndTarget can be NULL

 ******************************************************************************/

JBoolean
SyGGetDNDTarget
	(
	const JXContainer*	dndTarget,
	SyGFileTreeTable**	widget
	)
{
	// We can't compare dndTarget and widget because they aren't the same
	// when dropping on an iconified window.  So we simply insist that a
	// drag is in progress.  This insures that HandleDNDEnter/Leave() is
	// being called, so theDNDTarget is correct.

	assert( ((theApplication->GetCurrentDisplay())->GetDNDManager())->IsDragging() );

//	if (dndTarget != theDNDTarget)
//		{
//		theDNDTarget = NULL;
//		}

	*widget = theDNDTarget;
	return JI2B( theDNDTarget != NULL );
}

void
SyGSetDNDTarget
	(
	SyGFileTreeTable* widget
	)
{
	theDNDTarget = widget;
}

/******************************************************************************
 SyGGetDNDAskActions

 ******************************************************************************/

void
SyGGetDNDAskActions
	(
	JArray<Atom>*		actionList,
	JPtrArray<JString>*	descriptionList
	)
{
	JXDNDManager* dndMgr = ((JXGetApplication())->GetCurrentDisplay())->GetDNDManager();
	actionList->AppendElement(dndMgr->GetDNDActionCopyXAtom());
	actionList->AppendElement(dndMgr->GetDNDActionMoveXAtom());
	actionList->AppendElement(dndMgr->GetDNDActionLinkXAtom());

	JString* s = new JString(JGetString(kDNDCopyDescriptionID));
	assert( s != NULL );
	descriptionList->Append(s);

	s = new JString(JGetString(kDNDMoveDescriptionID));
	assert( s != NULL );
	descriptionList->Append(s);

	s = new JString(JGetString(kDNDLinkDescriptionID));
	assert( s != NULL );
	descriptionList->Append(s);
}

/******************************************************************************
 SyGGetWMClassInstance

 ******************************************************************************/

const JCharacter*
SyGGetWMClassInstance()
{
	return JGetString("SyGName");
}

const JCharacter*
SyGGetFolderWindowClass()
{
	return kFolderWindowClass;
}

/******************************************************************************
 SyGGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
SyGGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 SyGGetVersionStr

 ******************************************************************************/

JString
SyGGetVersionStr()
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

#include <jx_folder_large.xpm>
#include <jx_folder_selected_large.xpm>
#include <jx_folder_read_only_large.xpm>
#include <jx_folder_read_only_selected_large.xpm>
#include "syg_home_folder_large.xpm"
#include "syg_home_folder_selected_large.xpm"
#include "syg_floppy_folder_large.xpm"
#include "syg_floppy_folder_read_only_large.xpm"
#include "syg_cdrom_folder_large.xpm"
#include "syg_cdrom_folder_read_only_large.xpm"
#include <jx_trash_can_empty_large.xpm>
#include <jx_trash_can_empty_selected_large.xpm>
#include <jx_trash_can_full_large.xpm>
#include <jx_trash_can_full_selected_large.xpm>
#include <jx_hard_disk_large.xpm>
#include <jx_hard_disk_selected_large.xpm>
#include <jx_floppy_disk_large.xpm>
#include <jx_floppy_disk_selected_large.xpm>
#include <jx_cdrom_disk_large.xpm>
#include <jx_cdrom_disk_selected_large.xpm>

#include <jx_plain_file_small.xpm>
#include <jx_plain_file_selected_small.xpm>
#include <jx_folder_small.xpm>
#include <jx_folder_selected_small.xpm>
#include <jx_folder_read_only_small.xpm>
#include <jx_folder_read_only_selected_small.xpm>
#include <jx_folder_locked_small.xpm>
#include <jx_folder_locked_selected_small.xpm>
#include <jx_executable_small.xpm>
#include <jx_executable_selected_small.xpm>
#include <jx_unknown_file_small.xpm>
#include <jx_unknown_file_selected_small.xpm>

void
SyGCreateIcons()
{
	JXImageCache* c = theApplication->GetDisplay(1)->GetImageCache();

	SyGCreateMountPointSmallIcons();

	theFileIcon                   = c->GetImage(jx_plain_file_small);
	theSelectedFileIcon           = c->GetImage(jx_plain_file_selected_small);

	theFolderIcon                 = c->GetImage(jx_folder_small);
	theSelectedFolderIcon         = c->GetImage(jx_folder_selected_small);
	theReadOnlyFolderIcon         = c->GetImage(jx_folder_read_only_small);
	theSelectedReadOnlyFolderIcon = c->GetImage(jx_folder_read_only_selected_small);
	theLockedFolderIcon           = c->GetImage(jx_folder_locked_small);
	theSelectedLockedFolderIcon   = c->GetImage(jx_folder_locked_selected_small);

	theExecIcon                   = c->GetImage(jx_executable_small);
	theSelectedExecIcon           = c->GetImage(jx_executable_selected_small);
	theUnknownIcon                = c->GetImage(jx_unknown_file_small);
	theSelectedUnknownIcon        = c->GetImage(jx_unknown_file_selected_small);
}

void
SyGDeleteIcons()
{
	SyGDeleteMountPointSmallIcons();
}

JXImage*
SyGGetFileSmallIcon
	(
	const JBoolean selected
	)
{
	assert( theFileIcon != NULL );
	return selected ? theSelectedFileIcon : theFileIcon;
}

JXImage*
SyGGetFolderSmallIcon
	(
	const JBoolean selected
	)
{
	assert( theFolderIcon != NULL );
	return selected ? theSelectedFolderIcon : theFolderIcon;
}

JXImage*
SyGGetReadOnlyFolderSmallIcon
	(
	const JBoolean selected
	)
{
	assert( theReadOnlyFolderIcon != NULL );
	return selected ? theSelectedReadOnlyFolderIcon : theReadOnlyFolderIcon;
}

JXImage*
SyGGetLockedFolderSmallIcon
	(
	const JBoolean selected
	)
{
	assert( theLockedFolderIcon != NULL );
	return selected ? theSelectedLockedFolderIcon : theLockedFolderIcon;
}

JXImage*
SyGGetExecSmallIcon
	(
	const JBoolean selected
	)
{
	assert( theExecIcon != NULL );
	return selected ? theSelectedExecIcon : theExecIcon;
}

JXImage*
SyGGetUnknownSmallIcon
	(
	const JBoolean selected
	)
{
	assert( theExecIcon != NULL );
	return selected ? theSelectedUnknownIcon : theUnknownIcon;
}

JXImage*
SyGGetDirectorySmallIcon
	(
	const JCharacter* path
	)
{
	JMountType type;
	JXImage* image;
	if ((SyGGetApplication())->IsMountPoint(path, &type) &&
		SyGGetMountPointSmallIcon(type, &image))
		{
		return image;
		}
	else if (SyGIsTrashDirectory(path))
		{
		return SyGGetTrashSmallIcon();
		}
	else if (!JFSFileTreeNode::CanHaveChildren(path))
		{
		return SyGGetLockedFolderSmallIcon();
		}
	else if (!JDirectoryWritable(path))
		{
		return SyGGetReadOnlyFolderSmallIcon();
		}
	else
		{
		return SyGGetFolderSmallIcon();
		}
}

// Returns type value.  This value is arbitrary.  Do not store it in files.
// Never returns zero.  This value is reserved to mean "not set".

JIndex
SyGGetMountPointLargeIcon
	(
	const JCharacter*	path,
	SyGFileTreeList*	fileList,
	JXPM*				plainIcon,
	JXPM*				selectedIcon
	)
{
	JMountType type;
	const JBoolean isMP = theApplication->IsMountPoint(path, &type);

	if (!isMP)
		{
		JBoolean writable, isTop;
		JString device, fsTypeString;
		JFileSystemType fsType;
		if (JIsMounted(path, &writable, &isTop, &device, &fsType, &fsTypeString))
			{
			type = JGetUserMountPointType(path, device, fsTypeString);
			}
		else
			{
			type = kJHardDisk;
			}
		}

	const JBoolean writable = JDirectoryWritable(path);

	JString dir;
	if (JGetHomeDirectory(&dir) && JSameDirEntry(dir, path))
		{
		*plainIcon    = syg_home_folder_large;
		*selectedIcon = syg_home_folder_selected_large;
		return 1;
		}
	else if (SyGIsTrashDirectory(path))
		{
		(fileList->GetSyGFileTree())->Update();
		if (fileList->IsEmpty())
			{
			*plainIcon    = jx_trash_can_empty_large;
			*selectedIcon = jx_trash_can_empty_selected_large;
			return 2;
			}
		else
			{
			*plainIcon    = jx_trash_can_full_large;
			*selectedIcon = jx_trash_can_full_selected_large;
			return 3;
			}
		}
	else if (!isMP && type == kJFloppyDisk && !writable)
		{
		*plainIcon    = syg_floppy_folder_read_only_large;
		*selectedIcon = syg_floppy_folder_read_only_large;
		return 4;
		}
	else if (!isMP && type == kJFloppyDisk)
		{
		*plainIcon    = syg_floppy_folder_large;
		*selectedIcon = syg_floppy_folder_large;
		return 5;
		}
	else if (!isMP && type == kJCDROM && !writable)
		{
		*plainIcon    = syg_cdrom_folder_read_only_large;
		*selectedIcon = syg_cdrom_folder_read_only_large;
		return 4;
		}
	else if (!isMP && type == kJCDROM)
		{
		*plainIcon    = syg_cdrom_folder_large;
		*selectedIcon = syg_cdrom_folder_large;
		return 5;
		}
	else if (!isMP && !writable)
		{
		*plainIcon    = jx_folder_read_only_large;
		*selectedIcon = jx_folder_read_only_selected_large;
		return 6;
		}
	else if (!isMP)
		{
		*plainIcon    = jx_folder_large;
		*selectedIcon = jx_folder_selected_large;
		return 7;
		}
	else if (type == kJFloppyDisk)
		{
		*plainIcon    = jx_floppy_disk_large;
		*selectedIcon = jx_floppy_disk_selected_large;
		return 8;
		}
	else if (type == kJCDROM)
		{
		*plainIcon    = jx_cdrom_disk_large;
		*selectedIcon = jx_cdrom_disk_selected_large;
		return 9;
		}
	else
		{
		*plainIcon    = jx_hard_disk_large;
		*selectedIcon = jx_hard_disk_selected_large;
		return 11;
		}
}

/******************************************************************************
 SyGGetRecentFileDirectory

 ******************************************************************************/

JBoolean
SyGGetRecentFileDirectory
	(
	JString*		path,
	const JBoolean	reportErrors
	)
{
	if (!theRecentFileDir.IsEmpty())
		{
		*path = theRecentFileDir;
		return kJTrue;
		}

	if (!JGetPrefsDirectory(path))
		{
		if (reportErrors)
			{
			(JGetUserNotification())->ReportError(JGetString(kNoPrefsDirID));
			}
		return kJFalse;
		}

	*path = JCombinePathAndName(*path, kRecentFileDirName);

	JError err = JNoError();
	if (!JDirectoryExists(*path))
		{
		err = JCreateDirectory(*path, kRecentFileDirPerms);
		}
	else
		{
		err = JSetPermissions(*path, kRecentFileDirPerms);
		}

	if (err.OK())
		{
		theRecentFileDir = *path;
		return kJTrue;
		}
	else
		{
		path->Clear();
		if (reportErrors)
			{
			(JGetStringManager())->ReportError(kCreateRecentFileDirErrorID, err);
			}
		return kJFalse;
		}
}

/******************************************************************************
 SyGAddRecentFile

 ******************************************************************************/

void
SyGAddRecentFile
	(
	const JCharacter* fullname
	)
{
	JString recentDir;
	JString filename;
	JString path;
	if (SyGGetRecentFileDirectory(&recentDir) &&
		JSplitPathAndName(fullname, &path, &filename))
		{
		const JString recentFile = JCombinePathAndName(recentDir, filename);
		if (JNameUsed(recentFile))
			{
			JRemoveFile(recentFile);
			JCreateSymbolicLink(fullname, recentFile);
			return;
			}

		// remove oldest links such that only kRecentFileCount - 1 remain

		JDirInfo* info;
		if (JDirInfo::Create(recentDir, &info))
			{
			JBoolean changed = kJFalse;

			JSize count = info->GetEntryCount();
			for (JIndex i=1; i<=count; i++)
				{
				if (info->GetEntry(i).IsBrokenLink())
					{
					JRemoveFile(info->GetEntry(i).GetFullName());
					changed = kJTrue;
					}
				}

			if (changed)
				{
				info->ForceUpdate();
				}

			count = info->GetEntryCount();
			if (count >= kRecentFileCount)
				{
				info->ChangeSort(JDirEntry::CompareModTimes, JOrderedSetT::kSortDescending);
				for (JIndex i=count; i>=kRecentFileCount; i--)
					{
					JRemoveFile(info->GetEntry(i).GetFullName());
					}
				}

			// add new entry

			JCreateSymbolicLink(fullname, recentFile);
			}
		}
}
