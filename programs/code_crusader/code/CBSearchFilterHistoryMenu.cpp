/******************************************************************************
 CBSearchFilterHistoryMenu.cpp

	Maintains a fixed-length list of paths.  When the menu needs to be
	updated, non-existent directories are removed.

	BASE CLASS = JXHistoryMenuBase

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "CBSearchFilterHistoryMenu.h"
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchFilterHistoryMenu::CBSearchFilterHistoryMenu
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
}

CBSearchFilterHistoryMenu::CBSearchFilterHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXHistoryMenuBase(historyLength, owner, itemIndex, enclosure)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSearchFilterHistoryMenu::~CBSearchFilterHistoryMenu()
{
}

/******************************************************************************
 GetFilter

 ******************************************************************************/

const JString&
CBSearchFilterHistoryMenu::GetFilter
	(
	const Message&	message,
	bool*		invert
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const auto* selection =
		dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != nullptr );

	return GetFilter(selection->GetIndex(), invert);
}

const JString&
CBSearchFilterHistoryMenu::GetFilter
	(
	const JIndex	index,
	bool*		invert
	)
	const
{
	JString s;
	*invert = GetItemNMShortcut(index, &s) && s == JGetString("InvertFlag::CBSearchFilterHistoryMenu");

	return JXTextMenu::GetItemText(index);
}

/******************************************************************************
 AddFilter

	Prepend the given filter to the menu and remove outdated entries
	at the bottom.

 ******************************************************************************/

void
CBSearchFilterHistoryMenu::AddFilter
	(
	const JString&	filter,
	const bool	invert
	)
{
	if (!filter.IsEmpty())
		{
		AddItem(filter, invert ? JGetString("InvertFlag::CBSearchFilterHistoryMenu") : JString::empty);
		}
}
