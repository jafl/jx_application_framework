/******************************************************************************
 JXCurrentPathMenu.cpp

	When an item is selected, switch to the directory returned by GetPath()
	with the selected item index and then call SetPath().  SetPath() is not
	called automatically because your code should already be written to do
	it automatically whenever the path changes for *any* reason.

	The title of the menu is the name of the current directory.

	BASE CLASS = JXTextMenu

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "JXCurrentPathMenu.h"
#include <JXImage.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_folder_small.xpm>
#include <jx_folder_read_only_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXCurrentPathMenu::JXCurrentPathMenu
	(
	const JCharacter*	path,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu("", enclosure, hSizing, vSizing, x,y, w,h)
{
	JXCurrentPathMenuX(path);
}

JXCurrentPathMenu::JXCurrentPathMenu
	(
	const JCharacter*	path,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
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
	const JCharacter* path
	)
{
	JXDisplay*  d = GetDisplay();
	JXColormap* c = GetColormap();

	itsFolderIcon = new JXImage(d, c, jx_folder_small);
	assert( itsFolderIcon != NULL );

	itsReadOnlyFolderIcon = new JXImage(d, c, jx_folder_read_only_small);
	assert( itsReadOnlyFolderIcon != NULL );

	// after creating icons

	SetPath(path);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXCurrentPathMenu::~JXCurrentPathMenu()
{
	delete itsFolderIcon;
	delete itsReadOnlyFolderIcon;
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

	const JXMenu::ItemSelected* selection =
		dynamic_cast(const JXMenu::ItemSelected*, &message);
	assert( selection != NULL );

	return GetPath(selection->GetIndex());
}

JString
JXCurrentPathMenu::GetPath
	(
	const JIndex itemIndex
	)
	const
{
	JString path = GetItemText(1);
	for (JIndex i=2; i<=itemIndex; i++)
		{
		path = JCombinePathAndName(path, GetItemText(i));
		}
	JAppendDirSeparator(&path);
	return path;
}

/******************************************************************************
 SetPath

 ******************************************************************************/

void
JXCurrentPathMenu::SetPath
	(
	const JCharacter* path
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
		PrependItem(n);
		SetItemImage(1, GetIcon(p), kJFalse);
		p = p1;
		}

	PrependItem(p);
	SetItemImage(1, GetIcon(p), kJFalse);

	const JXImage* image = NULL;
	GetItemImage(GetItemCount(), &image);
	SetTitle(GetItemText(GetItemCount()), const_cast<JXImage*>(image), kJFalse);
	SetUpdateAction(kDisableNone);
}

/******************************************************************************
 GetIcon (private)

 ******************************************************************************/

JXImage*
JXCurrentPathMenu::GetIcon
	(
	const JCharacter* path
	)
	const
{
	return (JDirectoryWritable(path) ? itsFolderIcon : itsReadOnlyFolderIcon);
}
