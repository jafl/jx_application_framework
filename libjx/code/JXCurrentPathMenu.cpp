/******************************************************************************
 JXCurrentPathMenu.cpp

	When an item is selected, switch to the directory returned by GetPath()
	with the selected item index and then call SetPath().  SetPath() is not
	called automatically because your code should already be written to do
	it automatically whenever the path changes for *any* reason.

	The title of the menu is the name of the current directory.

	BASE CLASS = JXTextMenu

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "JXCurrentPathMenu.h"
#include "JXImageCache.h"
#include "JXDisplay.h"
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

#include "jx_folder_small.xpm"
#include "jx_folder_read_only_small.xpm"
#include "jx_hard_disk_small.xpm"
#include "jx_floppy_disk_small.xpm"
#include "jx_cdrom_disk_small.xpm"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCurrentPathMenu::JXCurrentPathMenu
	(
	const JString&		path,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu(JString::empty, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXCurrentPathMenuX(path);
}

JXCurrentPathMenu::JXCurrentPathMenu
	(
	const JString&	path,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXCurrentPathMenuX(path);
}

// private

void
JXCurrentPathMenu::JXCurrentPathMenuX
	(
	const JString& path
	)
{
	JXImageCache* cache   = GetDisplay()->GetImageCache();
	itsFolderIcon         = cache->GetImage(jx_folder_small);
	itsReadOnlyFolderIcon = cache->GetImage(jx_folder_read_only_small);
	itsHDIcon             = cache->GetImage(jx_hard_disk_small);
	itsFDIcon             = cache->GetImage(jx_floppy_disk_small);
	itsCDIcon             = cache->GetImage(jx_cdrom_disk_small);

	itsMountPointList = jnew JMountPointList(JPtrArrayT::kDeleteAll);
	JGetUserMountPointList(itsMountPointList, &itsMountPointState);

	// after creating icons

	SetPath(path);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCurrentPathMenu::~JXCurrentPathMenu()
{
}

/******************************************************************************
 GetPath

 ******************************************************************************/

JString
JXCurrentPathMenu::GetPath
	(
	const Message& message
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const auto* selection =
		dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != nullptr );

	return GetPath(selection->GetIndex());
}

JString
JXCurrentPathMenu::GetPath
	(
	const JIndex itemIndex
	)
	const
{
	if (itemIndex <= itsSegmentCount)
	{
		JString path = GetItemText(itsSegmentCount);
		for (JIndex i=itsSegmentCount-1; i>=itemIndex; i--)
		{
			path = JCombinePathAndName(path, GetItemText(i));
		}
		JAppendDirSeparator(&path);
		return path;
	}
	else
	{
		return GetItemText(itemIndex);
	}
}

/******************************************************************************
 SetPath

 ******************************************************************************/

void
JXCurrentPathMenu::SetPath
	(
	const JString& path
	)
{
	RemoveAllItems();

	JString p = path;
	JCleanPath(&p);

	JString p1, n;
	while (!JIsRootDirectory(p))
	{
		JStripTrailingDirSeparator(&p);
		JSplitPathAndName(p, &p1, &n);
		AppendItem(n);
		SetItemImage(GetItemCount(), GetIcon(p), false);
		p = p1;
	}

	AppendItem(p);
	SetItemImage(GetItemCount(), GetIcon(p), false);

	itsSegmentCount = GetItemCount();
	if (!itsMountPointList->IsEmpty())
	{
		ShowSeparatorAfter(itsSegmentCount);

		for (const auto& mp : *itsMountPointList)
		{
			AppendItem(*mp.path);
			SetItemImage(GetItemCount(), GetMountPointIcon(mp.type), false);
		}
	}

	const JXImage* image = nullptr;
	GetItemImage(1, &image);
	SetTitle(GetItemText(1), const_cast<JXImage*>(image), false);
	SetUpdateAction(kDisableNone);
}

/******************************************************************************
 GetIcon (private)

 ******************************************************************************/

JXImage*
JXCurrentPathMenu::GetIcon
	(
	const JString& path
	)
	const
{
	return JDirectoryWritable(path) ? itsFolderIcon : itsReadOnlyFolderIcon;
}

/******************************************************************************
 GetMountPointIcon (private)

 ******************************************************************************/

JXImage*
JXCurrentPathMenu::GetMountPointIcon
	(
	const JMountType type
	)
	const
{
	return (type == kJFloppyDisk ? itsFDIcon :
			type == kJCDROM      ? itsCDIcon :
			itsHDIcon);
}
