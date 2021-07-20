/******************************************************************************
 SyGGlobals.cpp

	Access to global objects and factories.

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "SyGGlobals.h"
#include "SyGPrefsMgr.h"
#include "SyGMDIServer.h"
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

static SyGApplication*			theApplication       = nullptr;	// owns itself
static SyGPrefsMgr*				thePrefsMgr          = nullptr;
static SyGMDIServer*			theMDIServer         = nullptr;	// owned by JX

static SyGViewManPageDialog*	theManPageDialog     = nullptr;	// not owned
static SyGFindFileDialog*		theFindFileDialog    = nullptr;	// not owned
static SyGChooseSaveFile*		theAltChooseSaveFile = nullptr;

static SyGFileTreeTable*		theDNDSource         = nullptr;	// not owned
static SyGFileTreeTable*		theDNDTarget         = nullptr;	// not owned

// owned by JXImageCache
static JXImage*	theFileIcon                   = nullptr;
static JXImage*	theSelectedFileIcon           = nullptr;
static JXImage*	theFolderIcon                 = nullptr;
static JXImage*	theSelectedFolderIcon         = nullptr;
static JXImage* theReadOnlyFolderIcon         = nullptr;
static JXImage* theSelectedReadOnlyFolderIcon = nullptr;
static JXImage* theLockedFolderIcon           = nullptr;
static JXImage* theSelectedLockedFolderIcon   = nullptr;
static JXImage*	theExecIcon                   = nullptr;
static JXImage*	theSelectedExecIcon           = nullptr;
static JXImage*	theUnknownIcon                = nullptr;
static JXImage*	theSelectedUnknownIcon        = nullptr;

// owned by JXImageCache
static JXImage* theHDSmallIcon  = nullptr;
static JXImage* theZipSmallIcon = nullptr;
static JXImage* theFDSmallIcon  = nullptr;
static JXImage* theCDSmallIcon  = nullptr;

// owned by JXImageCache
static JXImage* theTrashEmptySmallIcon         = nullptr;
static JXImage* theTrashEmptySelectedSmallIcon = nullptr;
static JXImage* theTrashFullSmallIcon          = nullptr;
static JXImage* theTrashFullSelectedSmallIcon  = nullptr;

static const JString kTrashDirName("/.trashcan/", JString::kNoCopy);
static JString theTrashDir;			// only need to compute this once
static JDirInfo* theTrashDirInfo = nullptr;
static const JString kRecentFileDirName("/.systemg/recent_files/", JString::kNoCopy);
static JString theRecentFileDir;	// only need to compute this once
const JSize kRecentFileCount     = 20;

const long kTrashCanPerms      = 0700;
const long kRecentFileDirPerms = 0700;

static const JUtf8Byte* kFolderWindowClass = "SystemG_Folder";

// private functions

void	SyGCreateIcons();
void	SyGDeleteIcons();

/******************************************************************************
 SyGCreateGlobals

 ******************************************************************************/

bool
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
		oldPrefsFile = JCombinePathAndName(oldPrefsFile, JString(".gSystemG.pref", JString::kNoCopy));
		if (JFileExists(oldPrefsFile) &&
			(JPrefsFile::GetFullName(app->GetSignature(), &newPrefsFile)).OK() &&
			!JFileExists(newPrefsFile))
			{
			JRenameFile(oldPrefsFile, newPrefsFile);
			}
		}

	bool isNew;
	thePrefsMgr = jnew SyGPrefsMgr(&isNew);
	assert(thePrefsMgr != nullptr);

	JXInitHelp();

	auto* wdMgr = jnew JXWDManager(app->GetCurrentDisplay(), true);
	assert( wdMgr != nullptr );
	// registers itself

	theMDIServer = jnew SyGMDIServer;
	assert( theMDIServer != nullptr );

	theManPageDialog = jnew SyGViewManPageDialog(JXGetPersistentWindowOwner());
	assert( theManPageDialog != nullptr );

	theFindFileDialog = jnew SyGFindFileDialog(JXGetPersistentWindowOwner());
	assert( theFindFileDialog != nullptr );

	theAltChooseSaveFile = jnew SyGChooseSaveFile(thePrefsMgr, kSAltCSSetupID);
	assert( theAltChooseSaveFile != nullptr );

	JString trashDir;
	SyGGetTrashDirectory(&trashDir, false);	// silently creates it

	return isNew;
}

/******************************************************************************
 SyGDeleteGlobals

 ******************************************************************************/

void
SyGDeleteGlobals()
{
	theMDIServer = nullptr;

	// window closed by JXGetPersistentWindowOwner()

	theManPageDialog->JPrefObject::WritePrefs();
	theManPageDialog = nullptr;

	theFindFileDialog->JPrefObject::WritePrefs();
	theFindFileDialog = nullptr;

	jdelete theAltChooseSaveFile;
	theAltChooseSaveFile = nullptr;

	jdelete theTrashDirInfo;
	theTrashDirInfo = nullptr;

	SyGDeleteIcons();

	JXFSBindingManager::Destroy();

	theApplication = nullptr;
	theDNDSource   = nullptr;
	theDNDTarget   = nullptr;

	// this must be last so everybody else can use it to save their setup

	jdelete thePrefsMgr;
	thePrefsMgr = nullptr;
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
	assert( theApplication != nullptr );
	return theApplication;
}

/******************************************************************************
 GGetPrefsMgr

 ******************************************************************************/

SyGPrefsMgr*
SyGGetPrefsMgr()
{
	assert(thePrefsMgr != nullptr);
	return thePrefsMgr;
}

/******************************************************************************
 SyGGetMDIServer

 ******************************************************************************/

SyGMDIServer*
SyGGetMDIServer()
{
	assert( theMDIServer != nullptr );
	return theMDIServer;
}

/******************************************************************************
 SyGIsTrashDirectory

 ******************************************************************************/

bool
SyGIsTrashDirectory
	(
	const JString& path
	)
{
	JString dir;
	return SyGGetTrashDirectory(&dir, false) &&
				JSameDirEntry(dir, path);
}

/******************************************************************************
 SyGGetTrashDirectory

 ******************************************************************************/

bool
SyGGetTrashDirectory
	(
	JString*		path,
	const bool	reportErrors
	)
{
	if (!theTrashDir.IsEmpty())
		{
		*path = theTrashDir;
		return true;
		}

	if (!JGetPrefsDirectory(path))
		{
		if (reportErrors)
			{
			JGetUserNotification()->ReportError(JGetString("NoPrefsDir::SyGGlobals"));
			}
		return false;
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
		const bool ok = JDirInfo::Create(theTrashDir, &theTrashDirInfo);
		assert( ok );
		return true;
		}
	else
		{
		path->Clear();
		if (reportErrors)
			{
			const JUtf8Byte* map[] =
			{
				"name", kTrashDirName.GetBytes(),
				"err",  err.GetMessage().GetBytes()
			};
			const JString msg = JGetString("CreatePrefsDirError::SyGGlobals", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			}
		return false;
		}
}

/******************************************************************************
 SyGTrashDirectoryIsEmpty

 ******************************************************************************/

bool
SyGTrashDirectoryIsEmpty()
{
	if (theTrashDirInfo != nullptr)
		{
		theTrashDirInfo->Update();
		return theTrashDirInfo->IsEmpty();
		}
	else
		{
		return true;
		}
}

/******************************************************************************
 SyGUpdateTrash

 ******************************************************************************/

void
SyGUpdateTrash()
{
	if (theTrashDirInfo != nullptr)
		{
		theTrashDirInfo->ForceUpdate();
		(SyGGetApplication())->UpdateTrash();
		}
}

/******************************************************************************
 SyGEmptyTrashDirectory

 ******************************************************************************/

bool
SyGEmptyTrashDirectory()
{
	JString trashDir;
	const bool hasTrash = SyGGetTrashDirectory(&trashDir, false);
	if (hasTrash && JKillDirectory(trashDir))
		{
		JCreateDirectory(trashDir, kTrashCanPerms);
		SyGUpdateTrash();
		return true;
		}
	else if (hasTrash)
		{
		JGetUserNotification()->ReportError(JGetString("EmptyTrashError::SyGGlobals"));
		return false;
		}
	else
		{
		return true;
		}
}

/******************************************************************************
 SyGDeleteDirEntry

	Returns true if successful.

 ******************************************************************************/

bool
SyGDeleteDirEntry
	(
	const JString& fullName
	)
{
	const JString sysCmd = "rm -rf " + JPrepArgForExec(fullName);
	return SyGExec(sysCmd);
}

/******************************************************************************
 SyGExec

	Returns true if successful.

 ******************************************************************************/

bool
SyGExec
	(
	const JString&	cmd,
	const bool	report
	)
{
	JString errOutput;
	const JError err = JRunProgram(cmd, &errOutput);
	if (!err.OK() && report)
		{
		JGetUserNotification()->ReportError(err.GetMessage());
		}
	return err.OK();
}

/******************************************************************************
 SyGGetManPageDialog

 ******************************************************************************/

SyGViewManPageDialog*
SyGGetManPageDialog()
{
	assert( theManPageDialog != nullptr );
	return theManPageDialog;
}

/******************************************************************************
 SyGGetFindFileDialog

 ******************************************************************************/

SyGFindFileDialog*
SyGGetFindFileDialog()
{
	assert( theFindFileDialog != nullptr );
	return theFindFileDialog;
}

/******************************************************************************
 SyGGetChooseSaveFile

 ******************************************************************************/

SyGChooseSaveFile*
SyGGetChooseSaveFile()
{
	assert( theAltChooseSaveFile != nullptr );
	return theAltChooseSaveFile;
}

/******************************************************************************
 DND source

	dndSource can be nullptr

 ******************************************************************************/

bool
SyGGetDNDSource
	(
	const JXWidget*		dndSource,
	SyGFileTreeTable**	widget
	)
{
	if (dndSource != theDNDSource)
		{
		theDNDSource = nullptr;
		}

	*widget = theDNDSource;
	return theDNDSource != nullptr;
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

	dndTarget can be nullptr

 ******************************************************************************/

bool
SyGGetDNDTarget
	(
	const JXContainer*	dndTarget,
	SyGFileTreeTable**	widget
	)
{
	// We can't compare dndTarget and widget because they aren't the same
	// when dropping on an iconified window.  So we simply insist that a
	// drag is in progress.  This ensures that HandleDNDEnter/Leave() is
	// being called, so theDNDTarget is correct.

	assert( ((theApplication->GetCurrentDisplay())->GetDNDManager())->IsDragging() );

//	if (dndTarget != theDNDTarget)
//		{
//		theDNDTarget = nullptr;
//		}

	*widget = theDNDTarget;
	return theDNDTarget != nullptr;
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
	JXDNDManager* dndMgr = (JXGetApplication()->GetCurrentDisplay())->GetDNDManager();
	actionList->AppendElement(dndMgr->GetDNDActionCopyXAtom());
	actionList->AppendElement(dndMgr->GetDNDActionMoveXAtom());
	actionList->AppendElement(dndMgr->GetDNDActionLinkXAtom());

	auto* s = jnew JString(JGetString("DNDCopyDescription::SyGGlobals"));
	assert( s != nullptr );
	descriptionList->Append(s);

	s = jnew JString(JGetString("DNDMoveDescription::SyGGlobals"));
	assert( s != nullptr );
	descriptionList->Append(s);

	s = jnew JString(JGetString("DNDLinkDescription::SyGGlobals"));
	assert( s != nullptr );
	descriptionList->Append(s);
}

/******************************************************************************
 SyGGetWMClassInstance

 ******************************************************************************/

const JUtf8Byte*
SyGGetWMClassInstance()
{
	return JGetString("SyGName").GetBytes();
}

const JUtf8Byte*
SyGGetFolderWindowClass()
{
	return kFolderWindowClass;
}

/******************************************************************************
 SyGGetVersionNumberStr

 ******************************************************************************/

const JString&
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
	const JUtf8Byte* map[] =
		{
		"version",   JGetString("VERSION").GetBytes(),
		"copyright", JGetString("COPYRIGHT").GetBytes()
		};
	return JGetString("Description::SyGGlobals", map, sizeof(map));
}

/******************************************************************************
 Icons

 ******************************************************************************/

#include <jx_hard_disk_small.xpm>
#include <jx_zip_disk_small.xpm>
#include <jx_floppy_disk_small.xpm>
#include <jx_cdrom_disk_small.xpm>

#include <jx_trash_can_empty_small.xpm>
#include <jx_trash_can_empty_selected_small.xpm>
#include <jx_trash_can_full_small.xpm>
#include <jx_trash_can_full_selected_small.xpm>

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

	theHDSmallIcon  = c->GetImage(jx_hard_disk_small);
	theZipSmallIcon = c->GetImage(jx_zip_disk_small);
	theFDSmallIcon  = c->GetImage(jx_floppy_disk_small);
	theCDSmallIcon  = c->GetImage(jx_cdrom_disk_small);

	theTrashEmptySmallIcon         = c->GetImage(jx_trash_can_empty_small);
	theTrashEmptySelectedSmallIcon = c->GetImage(jx_trash_can_empty_selected_small);
	theTrashFullSmallIcon          = c->GetImage(jx_trash_can_full_small);
	theTrashFullSelectedSmallIcon  = c->GetImage(jx_trash_can_full_selected_small);

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
}

JXImage*
SyGGetFileSmallIcon
	(
	const bool selected
	)
{
	assert( theFileIcon != nullptr );
	return selected ? theSelectedFileIcon : theFileIcon;
}

JXImage*
SyGGetFolderSmallIcon
	(
	const bool selected
	)
{
	assert( theFolderIcon != nullptr );
	return selected ? theSelectedFolderIcon : theFolderIcon;
}

JXImage*
SyGGetReadOnlyFolderSmallIcon
	(
	const bool selected
	)
{
	assert( theReadOnlyFolderIcon != nullptr );
	return selected ? theSelectedReadOnlyFolderIcon : theReadOnlyFolderIcon;
}

JXImage*
SyGGetLockedFolderSmallIcon
	(
	const bool selected
	)
{
	assert( theLockedFolderIcon != nullptr );
	return selected ? theSelectedLockedFolderIcon : theLockedFolderIcon;
}

JXImage*
SyGGetExecSmallIcon
	(
	const bool selected
	)
{
	assert( theExecIcon != nullptr );
	return selected ? theSelectedExecIcon : theExecIcon;
}

JXImage*
SyGGetUnknownSmallIcon
	(
	const bool selected
	)
{
	assert( theExecIcon != nullptr );
	return selected ? theSelectedUnknownIcon : theUnknownIcon;
}

JXImage*
SyGGetDirectorySmallIcon
	(
	const JString& path
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

JXImage*
SyGGetTrashSmallIcon
	(
	const bool selected
	)
{
#if defined SYSTEM_G
	if (SyGTrashDirectoryIsEmpty())
#elif defined JX_FVWM2_TASKBAR
	if (SyGTBTrashDirectoryIsEmpty())
#endif
		{
		return selected ? theTrashEmptySelectedSmallIcon : theTrashEmptySmallIcon;
		}
	else
		{
		return selected ? theTrashFullSelectedSmallIcon : theTrashFullSmallIcon;
		}
}

bool
SyGGetMountPointSmallIcon
	(
	const JMountType	type,
	JXImage**			image
	)
{
	if (type == kJHardDisk)
		{
		*image = theHDSmallIcon;
		}
	else if (type == kJFloppyDisk)
		{
		*image = theFDSmallIcon;
		}
	else if (type == kJCDROM)
		{
		*image = theCDSmallIcon;
		}
	else
		{
		*image = nullptr;
		}
	return *image != nullptr;
}

// Returns type value.  This value is arbitrary.  Do not store it in files.
// Never returns zero.  This value is reserved to mean "not set".

JIndex
SyGGetMountPointLargeIcon
	(
	const JString&		path,
	SyGFileTreeList*	fileList,
	JXPM*				plainIcon,
	JXPM*				selectedIcon
	)
{
	JMountType type;
	const bool isMP = theApplication->IsMountPoint(path, &type);

	if (!isMP)
		{
		bool writable, isTop;
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

	const bool writable = JDirectoryWritable(path);

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

bool
SyGGetRecentFileDirectory
	(
	JString*		path,
	const bool	reportErrors
	)
{
	if (!theRecentFileDir.IsEmpty())
		{
		*path = theRecentFileDir;
		return true;
		}

	if (!JGetPrefsDirectory(path))
		{
		if (reportErrors)
			{
			JGetUserNotification()->ReportError(JGetString("NoPrefsDir::SyGGlobals"));
			}
		return false;
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
		return true;
		}
	else
		{
		path->Clear();
		if (reportErrors)
			{
			const JUtf8Byte* map[] =
			{
				"name", kRecentFileDirName.GetBytes(),
				"err",  err.GetMessage().GetBytes()
			};
			const JString msg = JGetString("CreatePrefsDirError::SyGGlobals", map, sizeof(map));
			JGetUserNotification()->ReportError(msg);
			}
		return false;
		}
}

/******************************************************************************
 SyGAddRecentFile

 ******************************************************************************/

void
SyGAddRecentFile
	(
	const JString& fullname
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
			bool changed = false;

			JSize count = info->GetEntryCount();
			for (JIndex i=1; i<=count; i++)
				{
				if (info->GetEntry(i).IsBrokenLink())
					{
					JRemoveFile(info->GetEntry(i).GetFullName());
					changed = true;
					}
				}

			if (changed)
				{
				info->ForceUpdate();
				}

			count = info->GetEntryCount();
			if (count >= kRecentFileCount)
				{
				info->ChangeSort(JDirEntry::CompareModTimes, JListT::kSortDescending);
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
