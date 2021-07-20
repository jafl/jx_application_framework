/******************************************************************************
 CBSearchPathHistoryMenu.cpp

	Maintains a fixed-length list of paths.  When the menu needs to be
	updated, non-existent directories are removed.

	BASE CLASS = JXHistoryMenuBase

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "CBSearchPathHistoryMenu.h"
#include <JXDisplay.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_folder_small.xpm>

static const JString kOldRecurseFlag("(recurse)");
static const JString kRecurseFlag("(include subdirs)");

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchPathHistoryMenu::CBSearchPathHistoryMenu
	(
	const JSize			historyLength,
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
	SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jx_folder_small), false);
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
	bool*		recurse
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const auto* selection =
		dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != nullptr );

	return GetPath(selection->GetIndex(), recurse);
}

const JString&
CBSearchPathHistoryMenu::GetPath
	(
	const JIndex	index,
	bool*		recurse
	)
	const
{
	JString s;
	*recurse = GetItemNMShortcut(index, &s) &&
					(s == kRecurseFlag || s == kOldRecurseFlag);

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
	const JString&	fullName,
	const bool	recurse
	)
{
	if (!fullName.IsEmpty())
		{
		const JString path = JConvertToHomeDirShortcut(fullName);

		AddItem(path, recurse ? kRecurseFlag : JString::empty);
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
			!JConvertToAbsolutePath(dirName, JString::empty, &fullName))
			{
			RemoveItem(i);
			}
		}
}
