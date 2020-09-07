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
#include <JXImageCache.h>
#include <jXGlobals.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_plain_file_small.xpm>
#include <jx_folder_small.xpm>
#include <jx_executable_small.xpm>
#include <jx_unknown_file_small.xpm>

static const JUtf8Byte* kEmptyMenuText       = "No files";
static const JUtf8Byte* kEmptyMenuTextSuffix = " %d";

// JBroadcaster message types

const JUtf8Byte* JXFSDirMenu::kFileSelected = "kFileSelected::JXFSDirMenu";

/******************************************************************************
 Constructor

 *****************************************************************************/

JXFSDirMenu::JXFSDirMenu
	(
	const JString&		path,
	const JString&		title,
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
	const JString&	path,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
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
	const JString&				title,
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
	const JString&	path,
	JXFSDirMenu*	parent,
	const JIndex	itemIndex
	)
	:
	JXTextMenu(parent, itemIndex, parent->GetEnclosure()),
	itsPath(path),
	itsDirInfo(nullptr),
	itsEntries(nullptr),
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
	itsDirInfo = nullptr;
	itsEntries = nullptr;
	itsParent  = nullptr;

	JXImageCache* cache = GetDisplay()->GetImageCache();
	itsFileIcon         = cache->GetImage(jx_plain_file_small);
	itsFolderIcon       = cache->GetImage(jx_folder_small);
	itsExecIcon         = cache->GetImage(jx_executable_small);
	itsUnknownIcon      = cache->GetImage(jx_unknown_file_small);

	JXFSDirMenuX1();
}

void
JXFSDirMenu::JXFSDirMenuX1()
{
	itsOwnsFileIcon = kJFalse;
	itsOwnsExecIcon = kJFalse;

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

	if (itsOwnsFileIcon)
		{
		jdelete itsFileIcon;
		}

	if (itsOwnsExecIcon)
		{
		jdelete itsExecIcon;
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
	if (itsDirInfo == nullptr)
		{
		if (itsPath.IsEmpty())
			{
			itsPath = JGetRootDirectory();	// when top level menu uses list of files
			}

		JDirInfo::Create(itsPath, &itsDirInfo);
		}

	*info = itsDirInfo;
	return JI2B(*info != nullptr);
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

	if (itsEntries == nullptr)
		{
		itsEntries = jnew JPtrArray<JDirEntry>(JPtrArrayT::kDeleteAll);
		assert( itsEntries != nullptr );
		}
	else
		{
		itsEntries->CleanOut();
		}

	RemoveAllItems();

	JString fullName;
	for (JString* f : fileNameList)
		{
		JDirEntry* entry = jnew JDirEntry(*f);
		assert( entry != nullptr );
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
	if (itsOwnsFileIcon)
		{
		jdelete itsFileIcon;
		}

	itsFileIcon = jnew JXImage(image);
	assert( itsFileIcon != nullptr );

	itsOwnsFileIcon = kJTrue;
}

void
JXFSDirMenu::SetFileIcon
	(
	const JXPM& data
	)
{
	if (itsOwnsFileIcon)
		{
		jdelete itsFileIcon;
		}

	itsFileIcon     = GetDisplay()->GetImageCache()->GetImage(data);
	itsOwnsFileIcon = kJFalse;
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
	if (itsOwnsExecIcon)
		{
		jdelete itsExecIcon;
		}

	itsExecIcon = jnew JXImage(image);
	assert( itsExecIcon != nullptr );

	itsOwnsExecIcon = kJTrue;
}

void
JXFSDirMenu::SetExecIcon
	(
	const JXPM& data
	)
{
	if (itsOwnsExecIcon)
		{
		jdelete itsExecIcon;
		}

	itsExecIcon     = GetDisplay()->GetImageCache()->GetImage(data);
	itsOwnsExecIcon = kJFalse;
}

/******************************************************************************
 SetEmptyMessage

 ******************************************************************************/

void
JXFSDirMenu::SetEmptyMessage
	(
	const JString& msg
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
		assert(info != nullptr);
		if (itsEntries != nullptr)
			{
			BroadcastIfTopLevel(itsEntries->GetElement(info->GetIndex())->GetFullName());
			}
		else if (itsDirInfo != nullptr)
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
	if (itsEntries != nullptr)
		{
		return;
		}

	if (itsDirInfo == nullptr)
		{
		JBoolean ok;
		if (itsParent != nullptr && itsParent->itsDirInfo != nullptr)
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
		if (itsParent != nullptr && itsParent->itsDirInfo != nullptr)
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

		for (const JDirEntry* entry : *itsDirInfo)
			{
			if (itsDeleteBrokenLinksFlag && entry->IsBrokenLink())
				{
				JRemoveFile(entry->GetFullName());
				}
			else
				{
				AppendEntry(*entry);
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

		AppendItem(name, kPlainType, JString::empty, path);
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
			assert( menu != nullptr );
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
	SetMenuItems(s.GetBytes());
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
	if (itsParent == nullptr)
		{
		Broadcast(FileSelected(filename));
		}
	else
		{
		itsParent->BroadcastIfTopLevel(filename);
		}
}
