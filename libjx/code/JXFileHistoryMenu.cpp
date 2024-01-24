/******************************************************************************
 JXFileHistoryMenu.cpp

	Maintains a fixed-length history of files, with the oldest at the bottom
	of the menu.

	The path is displayed as the shortcut, and the file name is displayed
	as the menu item text.  This separates them into 2 columns so the user
	can ignore the path unless it is the deciding factor. Some tricks are
	used to be compatible with JXTextMenu, so always use GetFile() to get
	the file name.

	The default icon for each file is the "plain file."

	BASE CLASS = JXHistoryMenuBase

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXFileHistoryMenu.h"
#include "JXImageCache.h"
#include "JXDisplay.h"
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

#include "jx_plain_file_small.xpm"

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFileHistoryMenu::JXFileHistoryMenu
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
	JXHistoryMenuBase(historyLength, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXFileHistoryMenuX();
}

JXFileHistoryMenu::JXFileHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXHistoryMenuBase(historyLength, owner, itemIndex, enclosure)
{
	JXFileHistoryMenuX();
}

// private

void
JXFileHistoryMenu::JXFileHistoryMenuX()
{
	SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jx_plain_file_small), false);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileHistoryMenu::~JXFileHistoryMenu()
{
}

/******************************************************************************
 GetFile

 ******************************************************************************/

JString
JXFileHistoryMenu::GetFile
	(
	const Message& message
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const auto* selection =
		dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != nullptr );

	return GetFile(selection->GetIndex());
}

JString
JXFileHistoryMenu::GetFile
	(
	const JIndex index
	)
	const
{
	JString path;
	const bool ok = GetItemNMShortcut(index, &path);
	assert( ok );
	path.TrimWhitespace();

	return JCombinePathAndName(path, JXTextMenu::GetItemText(index));
}

/******************************************************************************
 AddFile

	Prepend the given file to the menu and remove outdated entries
	at the bottom.

 ******************************************************************************/

void
JXFileHistoryMenu::AddFile
	(
	const JString& fullName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);
	AddFile(path, name);
}

void
JXFileHistoryMenu::AddFile
	(
	const JString& origPath,
	const JString& name
	)
{
	if (origPath.IsEmpty() || name.IsEmpty())
	{
		return;
	}

	JString path(" ");
	path.Append(origPath);
	path.Append(" ");

	AddItem(name, path);
}

/******************************************************************************
 UpdateMenu (virtual protected)

 ******************************************************************************/

void
JXFileHistoryMenu::UpdateMenu()
{
	RemoveNonexistentFiles();
	JXHistoryMenuBase::UpdateMenu();
}

/******************************************************************************
 RemoveNonexistentFiles (private)

 ******************************************************************************/

void
JXFileHistoryMenu::RemoveNonexistentFiles()
{
	JString fullName;
	const JSize count       = GetItemCount();
	const JIndex firstIndex = GetFirstIndex();
	for (JIndex i=count; i>=firstIndex; i--)
	{
		fullName = GetFile(i);
		if (!JFileExists(fullName))
		{
			RemoveItem(i);
		}
	}
}
