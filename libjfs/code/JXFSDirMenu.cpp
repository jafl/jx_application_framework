/******************************************************************************
 JXFSDirMenu.cpp

	Uses hierarchical menus to provide access to file system.  Since this
	is an action menu, you should only listen for FileSelected, not
	NeedsUpdate or ItemSelected.

	GetDirInfo() is provided to allow you to set options such as a content
	filter.  All options that you set will automatically propagate to
	sub-menus.

	BASE CLASS = JXTextMenu

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "JXFSDirMenu.h"
#include <JXApplication.h>
#include <JXMenuManager.h>
#include <JXDisplay.h>
#include <JXImage.h>
#include <jXGlobals.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>
#include <jx_folder_small.xpm>
#include <jx_executable_small.xpm>
#include <jx_unknown_file_small.xpm>

static const JCharacter* kEmptyMenuText       = "No files";
static const JCharacter* kEmptyMenuTextSuffix = " %d";

// JBroadcaster message types

const JCharacter* JXFSDirMenu::kFileSelected = "kFileSelected::JXFSDirMenu";

/******************************************************************************
 Constructor

 *****************************************************************************/

JXFSDirMenu::JXFSDirMenu
	(
	const JCharacter*	path,
	const JCharacter*	title,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu(title, enclosure, hSizing, vSizing, x, y, w, h),
	itsPath(path),
	itsEmptyMsg(kEmptyMenuText)
{
	JXFSDirMenuX();
}

JXFSDirMenu::JXFSDirMenu
	(
	const JCharacter*	path,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure),
	itsPath(path),
	itsEmptyMsg(kEmptyMenuText)
{
	JXFSDirMenuX();
}

JXFSDirMenu::JXFSDirMenu
	(
	const JPtrArray<JString>&	fileNameList,
	const JCharacter*			title,
	JXContainer*				enclosure,
	const HSizingOption			hSizing,
	const VSizingOption			vSizing,
	const JCoordinate			x,
	const JCoordinate			y,
	const JCoordinate			w,
	const JCoordinate			h
	)
	:
	JXTextMenu(title, enclosure, hSizing, vSizing, x, y, w, h),
	itsEmptyMsg(kEmptyMenuText)
{
	JXFSDirMenuX();
	SetFileList(fileNameList);
}

JXFSDirMenu::JXFSDirMenu
	(
	const JPtrArray<JString>&	fileNameList,
	JXMenu*						owner,
	const JIndex				itemIndex,
	JXContainer*				enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure),
	itsEmptyMsg(kEmptyMenuText)
{
	JXFSDirMenuX();
	SetFileList(fileNameList);
}

// protected

JXFSDirMenu::JXFSDirMenu
	(
	const JCharacter*	path,
	JXFSDirMenu*		parent,
	const JIndex		itemIndex
	)
	:
	JXTextMenu(parent, itemIndex, parent->GetEnclosure()),
	itsPath(path),
	itsDirInfo(NULL),
	itsEntries(NULL),
	itsParent(parent),
	itsEmptyMsg(parent->itsEmptyMsg),
	itsFileIcon(parent->itsFileIcon),
	itsFolderIcon(parent->itsFolderIcon),
	itsExecIcon(parent->itsExecIcon),
	itsUnknownIcon(parent->itsUnknownIcon)
{
	JXFSDirMenuX1();
}

// private

void
JXFSDirMenu::JXFSDirMenuX()
{
	itsDirInfo = NULL;
	itsEntries = NULL;
	itsParent  = NULL;

	itsFileIcon = jnew JXImage(GetDisplay(), jx_plain_file_small);
	assert( itsFileIcon != NULL );

	itsFolderIcon = jnew JXImage(GetDisplay(), jx_folder_small);
	assert( itsFolderIcon != NULL );

	itsExecIcon = jnew JXImage(GetDisplay(), jx_executable_small);
	assert( itsExecIcon != NULL );

	itsUnknownIcon = jnew JXImage(GetDisplay(), jx_unknown_file_small);
	assert( itsUnknownIcon != NULL );

	JXFSDirMenuX1();
}

void
JXFSDirMenu::JXFSDirMenuX1()
{
	itsShowPathFlag          = kJFalse;
	itsDereferenceLinksFlag  = kJFalse;
	itsDeleteBrokenLinksFlag = kJFalse;

	CompressHeight();
	SetUpdateAction(JXMenu::kDisableNone);
	ClearMenu();
	ListenTo(this);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXFSDirMenu::~JXFSDirMenu()
{
	jdelete itsDirInfo;
	jdelete itsEntries;

	if (itsParent == NULL)
		{
		jdelete itsFileIcon;
		jdelete itsFolderIcon;
		jdelete itsExecIcon;
		jdelete itsUnknownIcon;
		}
}

/******************************************************************************
 GetDirInfo

	Allows clients to set options.  All settings automatically propagate to
	sub-menus.

 ******************************************************************************/

JBoolean
JXFSDirMenu::GetDirInfo
	(
	JDirInfo** info
	)
{
	if (itsDirInfo == NULL)
		{
		if (itsPath.IsEmpty())
			{
			itsPath = JGetRootDirectory();	// when top level menu uses list of files
			}

		JDirInfo::Create(itsPath, &itsDirInfo);
		}

	*info = itsDirInfo;
	return JI2B(*info != NULL);
}

/******************************************************************************
 SetFileList

 ******************************************************************************/

void
JXFSDirMenu::SetFileList
	(
	const JPtrArray<JString>& fileNameList
	)
{
	if (fileNameList.IsEmpty())		// preserve "No files" if empty
		{
		return;
		}

	if (itsEntries == NULL)
		{
		itsEntries = jnew JPtrArray<JDirEntry>(JPtrArrayT::kDeleteAll);
		assert( itsEntries != NULL );
		}
	else
		{
		itsEntries->CleanOut();
		}

	RemoveAllItems();
	const JSize count = fileNameList.GetElementCount();
	JString fullName;
	for (JIndex i = 1; i <= count; i++)
		{
		JDirEntry* entry = jnew JDirEntry(*(fileNameList.GetElement(i)));
		assert( entry != NULL );
		itsEntries->Append(entry);
		AppendEntry(*entry);
		}
}

/******************************************************************************
 SetFileIcon

 ******************************************************************************/

void
JXFSDirMenu::SetFileIcon
	(
	const JXImage& image
	)
{
	jdelete itsFileIcon;
	itsFileIcon = jnew JXImage(image);
	assert( itsFileIcon != NULL );
}

void
JXFSDirMenu::SetFileIcon
	(
	const JXPM& data
	)
{
	jdelete itsFileIcon;
	itsFileIcon = jnew JXImage(GetDisplay(), data);
	assert( itsFileIcon != NULL );
}

/******************************************************************************
 SetExecIcon

 ******************************************************************************/

void
JXFSDirMenu::SetExecIcon
	(
	const JXImage& image
	)
{
	jdelete itsExecIcon;
	itsExecIcon = jnew JXImage(image);
	assert( itsExecIcon != NULL );
}

void
JXFSDirMenu::SetExecIcon
	(
	const JXPM& data
	)
{
	jdelete itsExecIcon;
	itsExecIcon = jnew JXImage(GetDisplay(), data);
	assert( itsExecIcon != NULL );
}

/******************************************************************************
 SetEmptyMessage

 ******************************************************************************/

void
JXFSDirMenu::SetEmptyMessage
	(
	const JCharacter* msg
	)
{
	itsEmptyMsg = msg;
	assert( !itsEmptyMsg.Contains("|") && !itsEmptyMsg.Contains("%") );
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFSDirMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateSelf();
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* info =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert(info != NULL);
		if (itsEntries != NULL)
			{
			BroadcastIfTopLevel(itsEntries->GetElement(info->GetIndex())->GetFullName());
			}
		else if (itsDirInfo != NULL)
			{
			BroadcastIfTopLevel(itsDirInfo->GetEntry(info->GetIndex()).GetFullName());
			}
		}
	else
		{
		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateSelf (private)

 ******************************************************************************/

void
JXFSDirMenu::UpdateSelf()
{
	if (itsEntries != NULL)
		{
		return;
		}

	if (itsDirInfo == NULL)
		{
		JBoolean ok;
		if (itsParent != NULL && itsParent->itsDirInfo != NULL)
			{
			// copy options

			ok = JDirInfo::Create(*(itsParent->itsDirInfo), itsPath, &itsDirInfo);
			}
		else
			{
			ok = JDirInfo::Create(itsPath, &itsDirInfo);
			}

		if (!ok)
			{
			GetMenuManager()->CloseCurrentMenus();
			return;
			}
		}
	else
		{
		if (itsParent != NULL && itsParent->itsDirInfo != NULL)
			{
			itsDirInfo->CopySettings(*(itsParent->itsDirInfo));
			}
		itsDirInfo->Update();
		}

	if (itsDirInfo->IsEmpty())		// show "No files"
		{
		ClearMenu();
		}
	else
		{
		RemoveAllItems();
		const JSize count = itsDirInfo->GetEntryCount();
		for (JIndex i = 1; i <= count; i++)
			{
			const JDirEntry& entry = itsDirInfo->GetEntry(i);
			if (itsDeleteBrokenLinksFlag && entry.IsBrokenLink())
				{
				JRemoveFile(entry.GetFullName());
				}
			else
				{
				AppendEntry(entry);
				}
			}

		GetDisplay()->DispatchCursor();	// manual because we are dragging
		}
}

/******************************************************************************
 AppendEntry (private)

 ******************************************************************************/

void
JXFSDirMenu::AppendEntry
	(
	const JDirEntry& entry
	)
{
	if (entry.IsFile() && itsShowPathFlag)
		{
		JString path = entry.GetPath();
		JString name = entry.GetName();

		const JString* link;
		if (itsDereferenceLinksFlag && entry.IsLink() &&
			entry.GetLinkName(&link))
			{
			JSplitPathAndName(*link, &path, &name);
			}

		AppendItem(name, kPlainType, NULL, path);
		}
	else
		{
		AppendItem(entry.GetName());
		}

	const JIndex i = GetItemCount();

	if (entry.IsDirectory())
		{
		if (entry.IsReadable())
			{
			JXFSDirMenu* menu = jnew JXFSDirMenu(entry.GetFullName(), this, i);
			assert( menu != NULL );
			}
		else
			{
			DisableItem(i);
			}
		SetItemImage(i, itsFolderIcon, kJFalse);
		}
	else if (entry.IsFile())
		{
		if (entry.IsExecutable())
			{
			SetItemImage(i, itsExecIcon, kJFalse);
			}
		else
			{
			SetItemImage(i, itsFileIcon, kJFalse);
			}
		}
	else
		{
		SetItemImage(i, itsUnknownIcon, kJFalse);
		}
}

/******************************************************************************
 ClearMenu (protected)

 ******************************************************************************/

void
JXFSDirMenu::ClearMenu()
{
	JString s  = itsEmptyMsg;
	s         += kEmptyMenuTextSuffix;
	SetMenuItems(s);
}

/******************************************************************************
 BroadcastIfTopLevel (protected)

 ******************************************************************************/

void
JXFSDirMenu::BroadcastIfTopLevel
	(
	const JString& filename
	)
{
	if (itsParent == NULL)
		{
		Broadcast(FileSelected(filename));
		}
	else
		{
		itsParent->BroadcastIfTopLevel(filename);
		}
}
