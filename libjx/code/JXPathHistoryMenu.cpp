/******************************************************************************
 JXPathHistoryMenu.cpp

	Maintains a fixed-length list of paths.  When the menu needs to be
	updated, non-existent directories are removed.

	BASE CLASS = JXStringHistoryMenu

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXPathHistoryMenu.h"
#include "JXImage.h"
#include "JXImageCache.h"
#include "JXDisplay.h"
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jMountUtil.h>
#include <jx-af/jcore/jAssert.h>

#include "jx_folder_small.xpm"
#include "jx_hard_disk_small.xpm"
#include "jx_floppy_disk_small.xpm"
#include "jx_cdrom_disk_small.xpm"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPathHistoryMenu::JXPathHistoryMenu
	(
	const JSize			historyLength,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStringHistoryMenu(historyLength, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXPathHistoryMenuX();
}

JXPathHistoryMenu::JXPathHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXStringHistoryMenu(historyLength, owner, itemIndex, enclosure)
{
	JXPathHistoryMenuX();
}

// private

void
JXPathHistoryMenu::JXPathHistoryMenuX()
{
	SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jx_folder_small), false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPathHistoryMenu::~JXPathHistoryMenu()
{
}

/******************************************************************************
 SetBasePath

	This is used if a relative path is stored.  It must be an absolute path.

 ******************************************************************************/

void
JXPathHistoryMenu::SetBasePath
	(
	const JString& path
	)
{
	if (path.IsEmpty())
	{
		ClearBasePath();
	}
	else
	{
		assert( JIsAbsolutePath(path) );
		itsBasePath = path;
	}
}

/******************************************************************************
 UpdateMenu (virtual protected)

 ******************************************************************************/

void
JXPathHistoryMenu::UpdateMenu()
{
	if (GetFirstIndex() == 1)
	{
		JMountPointList list(JPtrArrayT::kDeleteAll);
		if (JGetUserMountPointList(&list, nullptr))
		{
			const JSize count = list.GetItemCount();
			SetFirstIndex(count+1);

			for (JIndex i=count; i>=1; i--)
			{
				const JMountPoint mp = list.GetItem(i);
				PrependItem(*(mp.path));
				if (i == count)
				{
					ShowSeparatorAfter(1);
				}

				if (mp.type == kJHardDisk)
				{
					SetItemImage(1, jx_hard_disk_small);
				}
				else if (mp.type == kJFloppyDisk)
				{
					SetItemImage(1, jx_floppy_disk_small);
				}
				else if (mp.type == kJCDROM)
				{
					SetItemImage(1, jx_cdrom_disk_small);
				}
			}
		}
	}

	RemoveInvalidPaths();
	JXStringHistoryMenu::UpdateMenu();
}

/******************************************************************************
 RemoveInvalidPaths (private)

 ******************************************************************************/

void
JXPathHistoryMenu::RemoveInvalidPaths()
{
	JString fullName;

	const JSize count       = GetItemCount();
	const JIndex firstIndex = GetFirstIndex();
	for (JIndex i=count; i>=firstIndex; i--)
	{
		const JString& dirName = JXTextMenu::GetItemText(i);
		if (!JConvertToAbsolutePath(dirName, itsBasePath, &fullName))
		{
			RemoveItem(i);
		}
	}
}
