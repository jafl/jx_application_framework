/******************************************************************************
 CBSearchPathHistoryMenu.cpp

	Maintains a fixed-length list of paths.  When the menu needs to be
	updated, non-existent directories are removed.

	BASE CLASS = JXHistoryMenuBase

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSearchPathHistoryMenu.h"
#include <JXImage.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_folder_small.xpm>

static const JCharacter* kOldRecurseFlag = "(recurse)";
static const JCharacter* kRecurseFlag    = "(include subdirs)";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchPathHistoryMenu::CBSearchPathHistoryMenu
	(
	const JSize			historyLength,
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
	JXHistoryMenuBase(historyLength, title, enclosure, hSizing, vSizing, x,y, w,h)
{
	CBSearchPathHistoryMenuX();
}

CBSearchPathHistoryMenu::CBSearchPathHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXHistoryMenuBase(historyLength, owner, itemIndex, enclosure)
{
	CBSearchPathHistoryMenuX();
}

// private

void
CBSearchPathHistoryMenu::CBSearchPathHistoryMenuX()
{
	JXImage* icon = new JXImage(GetDisplay(), jx_folder_small);
	assert( icon != NULL );
	SetDefaultIcon(icon, kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchPathHistoryMenu::~CBSearchPathHistoryMenu()
{
}

/******************************************************************************
 GetPath

 ******************************************************************************/

const JString&
CBSearchPathHistoryMenu::GetPath
	(
	const Message&	message,
	JBoolean*		recurse
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const JXMenu::ItemSelected* selection =
		dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != NULL );

	return GetPath(selection->GetIndex(), recurse);
}

const JString&
CBSearchPathHistoryMenu::GetPath
	(
	const JIndex	index,
	JBoolean*		recurse
	)
	const
{
	JString s;
	*recurse = JI2B(GetItemNMShortcut(index, &s) &&
					(s == kRecurseFlag || s == kOldRecurseFlag));

	return JXTextMenu::GetItemText(index);
}

/******************************************************************************
 AddPath

	Prepend the given path to the menu and remove outdated entries
	at the bottom.

 ******************************************************************************/

void
CBSearchPathHistoryMenu::AddPath
	(
	const JCharacter*	fullName,
	const JBoolean		recurse
	)
{
	if (!JStringEmpty(fullName))
		{
		AddItem(fullName, recurse ? kRecurseFlag : "");
		}
}

/******************************************************************************
 UpdateMenu (virtual protected)

 ******************************************************************************/

void
CBSearchPathHistoryMenu::UpdateMenu()
{
	RemoveInvalidPaths();
	JXHistoryMenuBase::UpdateMenu();
}

/******************************************************************************
 RemoveInvalidPaths (private)

 ******************************************************************************/

void
CBSearchPathHistoryMenu::RemoveInvalidPaths()
{
	JString fullName;

	const JSize count       = GetItemCount();
	const JIndex firstIndex = GetFirstIndex();
	for (JIndex i=count; i>=firstIndex; i--)
		{
		const JString& dirName = JXTextMenu::GetItemText(i);
		if (JIsAbsolutePath(dirName) &&
			!JConvertToAbsolutePath(dirName, NULL, &fullName))
			{
			RemoveItem(i);
			}
		}
}
