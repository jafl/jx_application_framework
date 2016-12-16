/******************************************************************************
 SyGApplication.cpp

	BASE CLASS = JXApplication, JPrefObject

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "SyGApplication.h"
#include "SyGPrefsMgr.h"
#include "SyGMDIServer.h"
#include "SyGTreeDir.h"
#include "SyGFileTreeTable.h"
#include "SyGAboutDialog.h"
#include "SyGChooseSaveFile.h"
#include "SyGStringData.h"
#include "SyGGlobals.h"
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <JFSFileTreeNode.h>
#include <JSimpleProcess.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

static const JCharacter* kAppSignature = "systemg";

#ifdef _J_CYGWIN
static const JCharacter* kDefaultTermCmd       = "xterm -title $n -n $n";
static const JCharacter* kDefaultGitStatusCmd  = "xterm -e git citool";
static const JCharacter* kDefaultGitHistoryCmd = "xterm -e gitk --all";
#else
static const JCharacter* kDefaultTermCmd       = "gnome-terminal --working-directory=$p";
static const JCharacter* kDefaultGitStatusCmd  = "git citool";
static const JCharacter* kDefaultGitHistoryCmd = "gitk --all";
#endif

static const JCharacter* kDefaultPostCheckoutCmd = "jcc --reload-open";

const JFileVersion kCurrentPrefsVersion = 5;

	// version  5 adds itsGitHistoryCmd
	// version  4 adds itsGitStatusCmd
	// version  3 adds itsPostCheckoutCmd
	// version  2 adds itsTermCmd
	// version  1 adds itsMountPointPrefs

// JBroadcaster messages

const JCharacter* SyGApplication::kTrashNeedsUpdate = "TrashNeedsUpdate::SyGApplication";
const JCharacter* SyGApplication::kShortcutsChanged = "ShortcutsChanged::SyGApplication";

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGApplication::SyGApplication
	(
	int*		argc,
	char*		argv[],
	JBoolean*	displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kSyGDefaultStringData),
	JPrefObject(NULL, kSAppID),
	itsTermCmd(kDefaultTermCmd),
	itsGitStatusCmd(kDefaultGitStatusCmd),
	itsGitHistoryCmd(kDefaultGitHistoryCmd),
	itsPostCheckoutCmd(kDefaultPostCheckoutCmd)
{
	// Create itsWindowList first so DirectorClosed() won't crash when
	// warn that prefs are unreadable.

	itsWindowList = jnew JPtrArray<SyGTreeDir>(JPtrArrayT::kForgetAll);
	assert( itsWindowList != NULL );

	*displayAbout = SyGCreateGlobals(this);

	if (!*displayAbout)
		{
		*prevVersStr = (SyGGetPrefsMgr())->GetSystemGVersionStr();
		if (*prevVersStr == SyGGetVersionNumberStr())
			{
			prevVersStr->Clear();
			}
		else
			{
			*displayAbout = kJTrue;
			}
		}
	else
		{
		prevVersStr->Clear();
		}

	itsMountPointList = jnew JMountPointList(JPtrArrayT::kDeleteAll);
	assert( itsMountPointList != NULL );
	JGetUserMountPointList(itsMountPointList, &itsMountPointState);

	itsShortcutList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsShortcutList != NULL );
	itsShortcutList->SetCompareFunction(CompareShortcuts);
	itsShortcutList->SetSortOrder(JOrderedSetT::kSortAscending);

	itsMountPointPrefs = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsMountPointPrefs != NULL );

	SetPrefInfo(SyGGetPrefsMgr(), kSAppID);
	JPrefObject::ReadPrefs();

	ListenTo(itsShortcutList);	// don't write taskbar file unless it doesn't exist
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGApplication::~SyGApplication()
{
	StopListening(itsShortcutList);		// don't write empty taskbar file

	JPrefObject::WritePrefs();

	jdelete itsWindowList;
	jdelete itsShortcutList;
	jdelete itsMountPointPrefs;
	jdelete itsMountPointList;

	SyGDeleteGlobals();
}

/******************************************************************************
 OpenDirectory

 ******************************************************************************/

JBoolean
SyGApplication::OpenDirectory()
{
	JString path;
	if ((SyGGetChooseSaveFile())->ChooseRPath("Select path", NULL, NULL, &path))
		{
		return OpenDirectory(path);
		}
	else
		{
		return kJFalse;
		}
}

JBoolean
SyGApplication::OpenDirectory
	(
	const JString&	pathName,
	SyGTreeDir**	dir,
	JIndex*			row,
	const JBoolean	deiconify,
	const JBoolean	reportError,
	const JBoolean	forceNew,
	const JBoolean	clearSelection
	)
{
	if (dir != NULL)
		{
		*dir = NULL;
		}

	if (row != NULL)
		{
		*row = 0;
		}

	JString fixedName, trueName;
	if (!JExpandHomeDirShortcut(pathName, &fixedName) ||
		!JConvertToAbsolutePath(fixedName, NULL, &trueName))
		{
		if (reportError)
			{
			JString msg = "\"";
			msg += pathName;
			msg += "\" does not exist.";
			(JGetUserNotification())->ReportError(msg);
			}
		return kJFalse;
		}

	// if file, select it after opening the window

	JString selectName;
	if (JFileExists(trueName) ||
		!JDirectoryExists(trueName))	// broken link
		{
		JStripTrailingDirSeparator(&trueName);
		JString path;
		JSplitPathAndName(trueName, &path, &selectName);
		trueName = path;
		}

	// can't check this until after making sure that trueName is directory

	if (!JFSFileTreeNode::CanHaveChildren(trueName))
		{
		if (reportError)
			{
			JString msg = "Unable to read contents of \"";
			msg += pathName;
			msg += "\"";
			(JGetUserNotification())->ReportError(msg);
			}
		return kJFalse;
		}

	// resolve all .. in path

	JIndex i;
	JString p, p1;
	while (trueName.LocateSubstring("..", &i))
		{
		p = trueName.GetSubstring(1, i+1);
		if (!JGetTrueName(p, &p1))
			{
			if (reportError)
				{
				JString msg = "\"";
				msg += p;
				msg += "\" does not exist.";
				(JGetUserNotification())->ReportError(msg);
				}
			return kJFalse;
			}

		trueName.ReplaceSubstring(1, i+1, p1);
		}

	// check if window is already open

	JString ancestor = trueName, n;
	JPtrArray<JString> pathList(JPtrArrayT::kDeleteAll);
	while (!JIsRootDirectory(ancestor))
		{
		const JIndex count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JString name = (itsWindowList->NthElement(i))->GetDirectory();
			if (JSameDirEntry(name, ancestor))
				{
				SyGTreeDir* childDir = itsWindowList->NthElement(i);
				childDir->Activate();
				if (dir != NULL)
					{
					*dir = childDir;
					}

				JPoint cell;
				(childDir->GetTable())->SelectName(pathList, selectName, &cell, clearSelection);
				if (row != NULL)
					{
					*row = cell.y;
					}

				return kJTrue;
				}
			}

		if (forceNew)
			{
			break;
			}

		JStripTrailingDirSeparator(&ancestor);
		JSplitPathAndName(ancestor, &p, &n);
		ancestor = p;
		pathList.Prepend(n);
		}

	// create new window

	fixedName = trueName;
	JGetTrueName(fixedName, &trueName);

	SyGTreeDir* childDir = jnew SyGTreeDir(trueName);
	assert( childDir != NULL );

	if (deiconify)
		{
		childDir->GetWindow()->Deiconify();
		}
	childDir->Activate();

	JPoint cell;
	(childDir->GetTable())->SelectName(selectName, NULL, &cell);
	if (row != NULL)
		{
		*row = cell.y;
		}

	itsWindowList->Append(childDir);

	if (dir != NULL)
		{
		*dir = childDir;
		}
	return kJTrue;
}

/******************************************************************************
 GetWindowNames

 ******************************************************************************/

void
SyGApplication::GetWindowNames
	(
	JPtrArray<JString>* names
	)
{
	const JIndex count = itsWindowList->GetElementCount();
	JString name;
	for (JIndex i=1; i<=count; i++)
		{
		name = JConvertToHomeDirShortcut(
					(itsWindowList->NthElement(i))->GetDirectory());
		names->Append(name);
		}
}

/******************************************************************************
 DirectorClosed (virtual protected)

	We cannot use dynamic_cast because the destructor chain is partially
	finished.

 ******************************************************************************/

void
SyGApplication::DirectorClosed
	(
	JXDirector* theDirector
	)
{
	itsWindowList->Remove((SyGTreeDir*) theDirector);
	JXApplication::DirectorClosed(theDirector);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGApplication::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsShortcutList)
		{
		Broadcast(ShortcutsChanged());
		SyGWriteTaskBarSetup(*itsShortcutList, kJTrue);
		}
	else
		{
		JXApplication::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateMountPointList (private)

 ******************************************************************************/

void
SyGApplication::UpdateMountPointList()
	const
{
	if (JGetUserMountPointList(itsMountPointList, &itsMountPointState))
		{
		const_cast<SyGApplication*>(this)->Broadcast(ShortcutsChanged());
		}
}

/******************************************************************************
 UpdateShortcutMenu

 ******************************************************************************/

void
SyGApplication::UpdateShortcutMenu
	(
	JXTextMenu* menu
	)
	const
{
	UpdateMountPointList();

	JIndex shortcutIndex = 1;

	JSize count = itsMountPointList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JMountPoint mp = itsMountPointList->GetElement(i);
		menu->AppendItem(*(mp.path), JXMenu::kPlainType, NULL,
						 GetNMShortcut(&shortcutIndex), *(mp.path));

		JXImage* image;
		if (SyGGetMountPointSmallIcon(mp.type, &image))
			{
			menu->SetItemImage(menu->GetItemCount(), image, kJFalse);
			}
		}

	JString trashDir;
	if (SyGGetTrashDirectory(&trashDir, kJFalse))
		{
		menu->AppendItem("Trash", JXMenu::kPlainType, NULL,
						 GetNMShortcut(&shortcutIndex), trashDir);
		menu->SetItemImage(menu->GetItemCount(), SyGGetTrashSmallIcon(), kJFalse);
		}

	menu->ShowSeparatorAfter(menu->GetItemCount());

	JXImage* folderIcon = SyGGetFolderSmallIcon();

	count = itsShortcutList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* path = itsShortcutList->NthElement(i);
		menu->AppendItem(*path, JXMenu::kPlainType, NULL,
						 GetNMShortcut(&shortcutIndex), *path);
		menu->SetItemImage(menu->GetItemCount(), folderIcon, kJFalse);
		}
}

/******************************************************************************
 GetNMShortcut (private)

 ******************************************************************************/

static JString kShortcutStr = "Ctrl-X";

static const JCharacter kShortcutKey[] =
{
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

const JSize kShortcutKeyCount = sizeof(kShortcutKey) / sizeof(JCharacter);

const JCharacter*
SyGApplication::GetNMShortcut
	(
	JIndex* i
	)
	const
{
	if (*i <= kShortcutKeyCount)
		{
		kShortcutStr.SetCharacter(6, kShortcutKey[*i-1]);
		(*i)++;
		return kShortcutStr.GetCString();
		}
	else
		{
		return NULL;
		}
}

/******************************************************************************
 LoadToolBarDefaults

 ******************************************************************************/

void
SyGApplication::LoadToolBarDefaults
	(
	JXToolBar*		toolBar,
	JXTextMenu*		shorcutMenu,
	const JIndex	firstIndex
	)
{
	const JSize count = itsMountPointList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		toolBar->AppendButton(shorcutMenu, firstIndex + i-1);
		}
}

/******************************************************************************
 OpenShortcut

 ******************************************************************************/

void
SyGApplication::OpenShortcut
	(
	const JIndex index
	)
{
	const JString* path = NULL;

	JString trashDir;
	const JBoolean hasTrashDir = SyGGetTrashDirectory(&trashDir, kJFalse);

	const JSize mpCount         = itsMountPointList->GetElementCount();
	const JIndex shortcutOffset = mpCount + (hasTrashDir ? 1 : 0);
	if (index <= mpCount)
		{
		path = (itsMountPointList->GetElement(index)).path;
		}
	else if (hasTrashDir && index == mpCount+1)
		{
		path = &trashDir;
		}
	else
		{
		path = itsShortcutList->NthElement(index - shortcutOffset);
		}

	if (!OpenDirectory(*path, NULL, NULL, kJTrue, kJFalse) && index > shortcutOffset)
		{
		JString msg = "\"";
		msg += *path;
		msg += "\" does not exist or is unreadable.  Do you want to remove this shortcut?";
		if (JGetUserNotification()->AskUserYes(msg))
			{
			itsShortcutList->DeleteElement(index - shortcutOffset);
			}
		}
}

/******************************************************************************
 AddShortcut

 ******************************************************************************/

void
SyGApplication::AddShortcut
	(
	const JString& shortcut
	)
{
	JString* s = jnew JString(JConvertToHomeDirShortcut(shortcut));
	assert( s != NULL );
	if (!itsShortcutList->InsertSorted(s, kJFalse))
		{
		jdelete s;
		}
}

/******************************************************************************
 RemoveShortcut

 ******************************************************************************/

void
SyGApplication::RemoveShortcut
	(
	const JString& shortcut
	)
{
	JString s = JConvertToHomeDirShortcut(shortcut);
	JIndex i;
	if (itsShortcutList->SearchSorted(&s, JOrderedSetT::kAnyMatch, &i))
		{
		itsShortcutList->DeleteElement(i);
		}
}

/******************************************************************************
 IsMountPoint

	If returns kJTrue and type != NULL, type is the type of the device.

 ******************************************************************************/

JBoolean
SyGApplication::IsMountPoint
	(
	const JCharacter*	path,
	JMountType*			type
	)
	const
{
	if (!JNameUsed(path))
		{
		return kJFalse;
		}

	UpdateMountPointList();

	const JSize count = itsMountPointList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JMountPoint info = itsMountPointList->GetElement(i);
		if (JSameDirEntry(path, *(info.path)))
			{
			if (type != NULL)
				{
				*type = info.type;
				}
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetMountPointPrefs

 ******************************************************************************/

JBoolean
SyGApplication::GetMountPointPrefs
	(
	const JCharacter*	path,
	const JString**		prefs
	)
	const
{
	JString s = GetMountPointPrefsPath(path);
	return itsMountPointPrefs->GetElement(s, prefs);
}

/******************************************************************************
 SetMountPointPrefs

 ******************************************************************************/

void
SyGApplication::SetMountPointPrefs
	(
	const JCharacter* path,
	const JCharacter* prefs
	)
{
	JString s = GetMountPointPrefsPath(path);
	itsMountPointPrefs->SetElement(s, prefs, JPtrArrayT::kDelete);
}

/******************************************************************************
 GetMountPointPrefsPath (private)

 ******************************************************************************/

JString
SyGApplication::GetMountPointPrefsPath
	(
	const JCharacter* path
	)
	const
{
	JString s = path;
	JCleanPath(&s);
	JStripTrailingDirSeparator(&s);
	return s;
}

/******************************************************************************
 Close (virtual protected)

 ******************************************************************************/

JBoolean
SyGApplication::Close()
{
	SaveProgramState();
	return JXApplication::Close();
}

/******************************************************************************
 RestoreProgramState

 ******************************************************************************/

JBoolean
SyGApplication::RestoreProgramState()
{
	JPtrArray<JString> children(JPtrArrayT::kDeleteAll);
	if (!(SyGGetPrefsMgr())->RestoreProgramState(&children))
		{
		return kJFalse;
		}

	const JSize count = children.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* str = children.NthElement(i);
		OpenDirectory(*str, NULL, NULL, kJFalse, kJFalse);
		}

	if (itsWindowList->IsEmpty())
		{
		JString path;
		if (!JGetHomeDirectory(&path) || !JFSFileTreeNode::CanHaveChildren(path))
			{
			path = JGetRootDirectory();
			}
		OpenDirectory(path);
		}

	return kJTrue;
}

/******************************************************************************
 SaveProgramState (private)

 ******************************************************************************/

void
SyGApplication::SaveProgramState()
{
	JPtrArray<JString> children(JPtrArrayT::kDeleteAll);
	const JSize count = itsWindowList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		children.Append((itsWindowList->NthElement(i))->GetDirectory());
		}
	(SyGGetPrefsMgr())->SaveProgramState(children);
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
SyGApplication::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
		{
		return;
		}

	input >> *itsShortcutList;

	if (vers >= 1)
		{
		input >> *itsMountPointPrefs;
		}

	if (vers >= 2)
		{
		input >> itsTermCmd;
		}

	if (vers >= 3)
		{
		input >> itsPostCheckoutCmd;
		}

	if (vers >= 4)
		{
		input >> itsGitStatusCmd;
		}

	if (vers >= 5)
		{
		input >> itsGitHistoryCmd;
		}

	SyGWriteTaskBarSetup(*itsShortcutList, kJFalse);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
SyGApplication::WritePrefs
	(
	std::ostream& output
	)
	const
{
	// flush unused mount point prefs

	JStringPtrMapCursor<JString> cursor(itsMountPointPrefs);
	const JSize mpCount = itsMountPointList->GetElementCount();

	JBoolean changed;
	do
		{
		changed = kJFalse;

		cursor.Reset();
		while (cursor.Next())
			{
			JBoolean found = kJFalse;

			const JCharacter* key = cursor.GetKey();
			for (JIndex i=1; i<=mpCount; i++)
				{
				const JMountPoint info = itsMountPointList->GetElement(i);
				if (JSameDirEntry(*(info.path), key))
					{
					found = kJTrue;
					break;
					}
				}

			if (!found)
				{
				itsMountPointPrefs->DeleteElement(key);
				changed = kJTrue;
				break;
				}
			}
		}
		while (changed);

	// write data

	output << kCurrentPrefsVersion;
	output << ' ' << *itsShortcutList;
	output << ' ' << *itsMountPointPrefs;
	output << ' ' << itsTermCmd;
	output << ' ' << itsPostCheckoutCmd;
	output << ' ' << itsGitStatusCmd;
	output << ' ' << itsGitHistoryCmd;
}

/******************************************************************************
 DisplayAbout

 ******************************************************************************/

void
SyGApplication::DisplayAbout
	(
	const JCharacter* prevVersStr
	)
{
	SyGAboutDialog* dlog = jnew SyGAboutDialog(this, prevVersStr);
	assert( dlog != NULL );
	dlog->BeginDialog();
}

/******************************************************************************
 OpenTerminal

 ******************************************************************************/

void
SyGApplication::OpenTerminal
	(
	const JCharacter* path
	)
{
	JString cmd = itsTermCmd, fullName = path, p, n;
	JStripTrailingDirSeparator(&fullName);
	JSplitPathAndName(fullName, &p, &n);
	n        = JPrepArgForExec(n);
	fullName = JPrepArgForExec(fullName);
	JIndex i;
	while (cmd.LocateSubstring("$n", &i))
		{
		cmd.ReplaceSubstring(i, i+1, n);
		}
	while (cmd.LocateSubstring("$p", &i))
		{
		cmd.ReplaceSubstring(i, i+1, fullName);
		}

	const JError err = JSimpleProcess::Create(path, cmd, kJTrue);
	err.ReportIfError();
}

/******************************************************************************
 DirectoryRenamed

 ******************************************************************************/

void
SyGApplication::DirectoryRenamed
	(
	const Message& message
	)
{
	Broadcast(message);
}

/******************************************************************************
 UpdateTrash

 ******************************************************************************/

void
SyGApplication::UpdateTrash()
{
	Broadcast(TrashNeedsUpdate());
}

/******************************************************************************
 CleanUpBeforeSuddenDeath (virtual protected)

 ******************************************************************************/

void
SyGApplication::CleanUpBeforeSuddenDeath
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JXApplication::CleanUpBeforeSuddenDeath(reason);

	if (reason != JXDocumentManager::kAssertFired)
		{
		JPrefObject::WritePrefs();
		SaveProgramState();

		const JSize count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsWindowList->NthElement(i))->SaveState();
			}
		}

	SyGCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JCharacter*
SyGApplication::GetAppSignature()
{
	return kAppSignature;
}

/******************************************************************************
 InitStrings (static)

	If we are going to print something to the command line and then quit,
	we haven't initialized JX, but we still need the string data.

 ******************************************************************************/

void
SyGApplication::InitStrings()
{
	(JGetStringManager())->Register(kAppSignature, kSyGDefaultStringData);
}

/******************************************************************************
 CompareShortcuts (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
SyGApplication::CompareShortcuts
	(
	JString* const & s1,
	JString* const & s2
	)
{
	const JBoolean h1 = s1->BeginsWith("~");
	const JBoolean h2 = s2->BeginsWith("~");
	if (h1 && !h2)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (!h1 && h2)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else
		{
		return JCompareStringsCaseInsensitive(s1, s2);
		}
}
