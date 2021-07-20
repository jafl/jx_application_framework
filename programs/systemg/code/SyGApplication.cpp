/******************************************************************************
 SyGApplication.cpp

	BASE CLASS = JXApplication, JPrefObject

	Copyright (C) 1997 by Glenn W. Bach.

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
#include <JStringIterator.h>
#include <jFileUtil.h>
#include <jAssert.h>

static const JUtf8Byte* kAppSignature = "systemg";

#ifdef _J_OSX
static const JString kDefaultTermCmd       ("open -a Terminal $p", JString::kNoCopy);
static const JString kDefaultGitStatusCmd  ("git gui", JString::kNoCopy);
static const JString kDefaultGitHistoryCmd ("gitk --all", JString::kNoCopy);
#elif defined _J_CYGWIN
static const JString kDefaultTermCmd       ("xterm -title $n -n $n", JString::kNoCopy);
static const JString kDefaultGitStatusCmd  ("xterm -e git gui", JString::kNoCopy);
static const JString kDefaultGitHistoryCmd ("xterm -e gitk --all", JString::kNoCopy);
#else
static const JString kDefaultTermCmd       ("gnome-terminal --working-directory=$p", JString::kNoCopy);
static const JString kDefaultGitStatusCmd  ("git gui", JString::kNoCopy);
static const JString kDefaultGitHistoryCmd ("gitk --all", JString::kNoCopy);
#endif

static const JString kDefaultPostCheckoutCmd("jcc --reload-open", JString::kNoCopy);

const JFileVersion kCurrentPrefsVersion = 5;

	// version  5 adds itsGitHistoryCmd
	// version  4 adds itsGitStatusCmd
	// version  3 adds itsPostCheckoutCmd
	// version  2 adds itsTermCmd
	// version  1 adds itsMountPointPrefs

// JBroadcaster messages

const JUtf8Byte* SyGApplication::kTrashNeedsUpdate = "TrashNeedsUpdate::SyGApplication";
const JUtf8Byte* SyGApplication::kShortcutsChanged = "ShortcutsChanged::SyGApplication";

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGApplication::SyGApplication
	(
	int*		argc,
	char*		argv[],
	bool*	displayAbout,
	JString*	prevVersStr
	)
	:
	JXApplication(argc, argv, kAppSignature, kSyGDefaultStringData),
	JPrefObject(nullptr, kSAppID),
	itsTermCmd(kDefaultTermCmd),
	itsGitStatusCmd(kDefaultGitStatusCmd),
	itsGitHistoryCmd(kDefaultGitHistoryCmd),
	itsPostCheckoutCmd(kDefaultPostCheckoutCmd)
{
	// Create itsWindowList first so DirectorClosed() won't crash when
	// warn that prefs are unreadable.

	itsWindowList = jnew JPtrArray<SyGTreeDir>(JPtrArrayT::kForgetAll);
	assert( itsWindowList != nullptr );

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
			*displayAbout = true;
			}
		}
	else
		{
		prevVersStr->Clear();
		}

	itsMountPointList = jnew JMountPointList(JPtrArrayT::kDeleteAll);
	assert( itsMountPointList != nullptr );
	JGetUserMountPointList(itsMountPointList, &itsMountPointState);

	itsShortcutList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsShortcutList != nullptr );
	itsShortcutList->SetCompareFunction(CompareShortcuts);
	itsShortcutList->SetSortOrder(JListT::kSortAscending);

	itsMountPointPrefs = jnew JStringPtrMap<JString>(JPtrArrayT::kDeleteAll);
	assert( itsMountPointPrefs != nullptr );

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

bool
SyGApplication::OpenDirectory()
{
	JString path;
	if (SyGGetChooseSaveFile()->ChooseRPath(
			JGetString("OpenDirectoryPrompt::SyGApplication"),
			JString::empty, JString::empty, &path))
		{
		return OpenDirectory(path);
		}
	else
		{
		return false;
		}
}

bool
SyGApplication::OpenDirectory
	(
	const JString&	pathName,
	SyGTreeDir**	dir,
	JIndex*			row,
	const bool	deiconify,
	const bool	reportError,
	const bool	forceNew,
	const bool	clearSelection
	)
{
	if (dir != nullptr)
		{
		*dir = nullptr;
		}

	if (row != nullptr)
		{
		*row = 0;
		}

	JString fixedName, trueName;
	if (!JExpandHomeDirShortcut(pathName, &fixedName) ||
		!JConvertToAbsolutePath(fixedName, JString::empty, &trueName))
		{
		if (reportError)
			{
			const JUtf8Byte* map[] =
			{
				"name", pathName.GetBytes()
			};
			JGetUserNotification()->ReportError(JGetString("PathDoesNotExist::SyGApplication", map, sizeof(map)));
			}
		return false;
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
			const JUtf8Byte* map[] =
			{
				"name", pathName.GetBytes()
			};
			JGetUserNotification()->ReportError(JGetString("Unreadable::SyGApplication", map, sizeof(map)));
			}
		return false;
		}

	// resolve all .. in path

	JString p1;

	JStringIterator iter(&trueName);
	iter.BeginMatch();
	while (iter.Next(".."))
		{
		const JString p = iter.FinishMatch(true).GetString();
		if (!JGetTrueName(p, &p1))
			{
			if (reportError)
				{
				const JUtf8Byte* map[] =
				{
					"name", p.GetBytes()
				};
				const JString msg = JGetString("PathDoesNotExist::SyGApplication", map, sizeof(map));
				JGetUserNotification()->ReportError(msg);
				}
			return false;
			}

		iter.ReplaceLastMatch(p1);
		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		iter.BeginMatch();
		}
	iter.Invalidate();

	JCleanPath(&trueName);

	// check if window is already open

	JString p;

	JString ancestor = trueName, n;
	JPtrArray<JString> pathList(JPtrArrayT::kDeleteAll);
	while (!JIsRootDirectory(ancestor))
		{
		const JIndex count = itsWindowList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const JString name = (itsWindowList->GetElement(i))->GetDirectory();
			if (JSameDirEntry(name, ancestor))
				{
				SyGTreeDir* childDir = itsWindowList->GetElement(i);
				childDir->Activate();
				if (dir != nullptr)
					{
					*dir = childDir;
					}

				JPoint cell;
				(childDir->GetTable())->SelectName(pathList, selectName, &cell, clearSelection);
				if (row != nullptr)
					{
					*row = cell.y;
					}

				return true;
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

	auto* childDir = jnew SyGTreeDir(trueName);
	assert( childDir != nullptr );

	if (deiconify)
		{
		childDir->GetWindow()->Deiconify();
		}
	childDir->Activate();

	JPoint cell;
	(childDir->GetTable())->SelectName(selectName, nullptr, &cell);
	if (row != nullptr)
		{
		*row = cell.y;
		}

	itsWindowList->Append(childDir);

	if (dir != nullptr)
		{
		*dir = childDir;
		}
	return true;
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
					(itsWindowList->GetElement(i))->GetDirectory());
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
		menu->AppendItem(*(mp.path), JXMenu::kPlainType, JString::empty,
						 GetNMShortcut(&shortcutIndex), *(mp.path));

		JXImage* image;
		if (SyGGetMountPointSmallIcon(mp.type, &image))
			{
			menu->SetItemImage(menu->GetItemCount(), image, false);
			}
		}

	JString trashDir;
	if (SyGGetTrashDirectory(&trashDir, false))
		{
		menu->AppendItem(JGetString("TrashName::SyGGlobals"), JXMenu::kPlainType, JString::empty,
						 GetNMShortcut(&shortcutIndex), trashDir);
		menu->SetItemImage(menu->GetItemCount(), SyGGetTrashSmallIcon(), false);
		}

	menu->ShowSeparatorAfter(menu->GetItemCount());

	JXImage* folderIcon = SyGGetFolderSmallIcon();

	count = itsShortcutList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* path = itsShortcutList->GetElement(i);
		menu->AppendItem(*path, JXMenu::kPlainType, JString::empty,
						 GetNMShortcut(&shortcutIndex), *path);
		menu->SetItemImage(menu->GetItemCount(), folderIcon, false);
		}
}

/******************************************************************************
 GetNMShortcut (private)

 ******************************************************************************/

static JString kShortcutPrefix("Ctrl-");

static const JUtf8Byte* kShortcutKey[] =
{
	"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
};

const JSize kShortcutKeyCount = sizeof(kShortcutKey) / sizeof(JUtf8Byte*);

JString
SyGApplication::GetNMShortcut
	(
	JIndex* i
	)
	const
{
	if (*i <= kShortcutKeyCount)
		{
		JString s = kShortcutPrefix;
		s += kShortcutKey[(*i)++ - 1];
		return s;
		}
	else
		{
		return JString::empty;
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
	const JString* path = nullptr;

	JString trashDir;
	const bool hasTrashDir = SyGGetTrashDirectory(&trashDir, false);

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
		path = itsShortcutList->GetElement(index - shortcutOffset);
		}

	if (!OpenDirectory(*path, nullptr, nullptr, true, false) && index > shortcutOffset)
		{
		const JUtf8Byte* map[] =
		{
			"name", path->GetBytes()
		};
		const JString msg = JGetString("InvalidShortcut::SyGApplication", map, sizeof(map));
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
	auto* s = jnew JString(JConvertToHomeDirShortcut(shortcut));
	assert( s != nullptr );
	if (!itsShortcutList->InsertSorted(s, false))
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
	if (itsShortcutList->SearchSorted(&s, JListT::kAnyMatch, &i))
		{
		itsShortcutList->DeleteElement(i);
		}
}

/******************************************************************************
 IsMountPoint

	If returns true and type != nullptr, type is the type of the device.

 ******************************************************************************/

bool
SyGApplication::IsMountPoint
	(
	const JString&	path,
	JMountType*		type
	)
	const
{
	if (!JNameUsed(path))
		{
		return false;
		}

	UpdateMountPointList();

	const JSize count = itsMountPointList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JMountPoint info = itsMountPointList->GetElement(i);
		if (JSameDirEntry(path, *(info.path)))
			{
			if (type != nullptr)
				{
				*type = info.type;
				}
			return true;
			}
		}

	return false;
}

/******************************************************************************
 GetMountPointPrefs

 ******************************************************************************/

bool
SyGApplication::GetMountPointPrefs
	(
	const JString&	path,
	const JString**	prefs
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
	const JString&	path,
	const JString&	prefs
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
	const JString& path
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

bool
SyGApplication::Close()
{
	SaveProgramState();
	return JXApplication::Close();
}

/******************************************************************************
 RestoreProgramState

 ******************************************************************************/

bool
SyGApplication::RestoreProgramState()
{
	JPtrArray<JString> children(JPtrArrayT::kDeleteAll);
	if (!(SyGGetPrefsMgr())->RestoreProgramState(&children))
		{
		return false;
		}

	const JSize count = children.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* str = children.GetElement(i);
		OpenDirectory(*str, nullptr, nullptr, false, false);
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

	return true;
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
		children.Append((itsWindowList->GetElement(i))->GetDirectory());
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

	bool changed;
	do
		{
		changed = false;

		cursor.Reset();
		while (cursor.Next())
			{
			bool found = false;

			const JString& key = cursor.GetKey();
			for (JIndex i=1; i<=mpCount; i++)
				{
				const JMountPoint info = itsMountPointList->GetElement(i);
				if (JSameDirEntry(*(info.path), key))
					{
					found = true;
					break;
					}
				}

			if (!found)
				{
				itsMountPointPrefs->DeleteElement(key);
				changed = true;
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
	const JString& prevVersStr
	)
{
	auto* dlog = jnew SyGAboutDialog(this, prevVersStr);
	assert( dlog != nullptr );
	dlog->BeginDialog();
}

/******************************************************************************
 OpenTerminal

 ******************************************************************************/

void
SyGApplication::OpenTerminal
	(
	const JString& path
	)
{
	JString cmd = itsTermCmd, fullName = path, p, n;
	JStripTrailingDirSeparator(&fullName);
	JSplitPathAndName(fullName, &p, &n);
	n        = JPrepArgForExec(n);
	fullName = JPrepArgForExec(fullName);

	JStringIterator iter(&cmd);
	while (iter.Next("$n"))
		{
		iter.ReplaceLastMatch(n);
		}

	iter.MoveTo(kJIteratorStartAtBeginning, 0);
	while (iter.Next("$p"))
		{
		iter.ReplaceLastMatch(fullName);
		}
	iter.Invalidate();

	const JError err = JSimpleProcess::Create(path, cmd, true);
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
			(itsWindowList->GetElement(i))->SaveState();
			}
		}

	SyGCleanUpBeforeSuddenDeath(reason);		// must be last call
}

/******************************************************************************
 GetAppSignature (static)

 ******************************************************************************/

const JUtf8Byte*
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
	JGetStringManager()->Register(kAppSignature, kSyGDefaultStringData);
}

/******************************************************************************
 CompareShortcuts (static private)

 ******************************************************************************/

JListT::CompareResult
SyGApplication::CompareShortcuts
	(
	JString* const & s1,
	JString* const & s2
	)
{
	const bool h1 = s1->BeginsWith("~");
	const bool h2 = s2->BeginsWith("~");
	if (h1 && !h2)
		{
		return JListT::kFirstLessSecond;
		}
	else if (!h1 && h2)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return JCompareStringsCaseInsensitive(s1, s2);
		}
}
