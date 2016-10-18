/******************************************************************************
 CBSearchFilterHistoryMenu.cpp

	Maintains a fixed-length list of paths.  When the menu needs to be
	updated, non-existent directories are removed.

	BASE CLASS = JXHistoryMenuBase

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSearchFilterHistoryMenu.h"
#include <jAssert.h>

static const JCharacter* kInvertFlag = "(invert)";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBSearchFilterHistoryMenu::CBSearchFilterHistoryMenu
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
	JBoolean*		invert
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const JXMenu::ItemSelected* selection =
		dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != NULL );

	return GetFilter(selection->GetIndex(), invert);
}

const JString&
CBSearchFilterHistoryMenu::GetFilter
	(
	const JIndex	index,
	JBoolean*		invert
	)
	const
{
	JString s;
	*invert = JI2B(GetItemNMShortcut(index, &s) && s == kInvertFlag);

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
	const JCharacter*	filter,
	const JBoolean		invert
	)
{
	if (!JString::IsEmpty(filter))
		{
		AddItem(filter, invert ? kInvertFlag : "");
		}
}
