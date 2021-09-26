/******************************************************************************
 JXStringHistoryMenu.cpp

	Maintains a fixed-length history of strings, with the oldest at the bottom
	of the menu.

	It makes no sense to use this as a PopupChoice menu because it should be
	coupled to a JXInputField.

	BASE CLASS = JXHistoryMenuBase

	Copyright (C) 1997-98 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXStringHistoryMenu.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStringHistoryMenu::JXStringHistoryMenu
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

JXStringHistoryMenu::JXStringHistoryMenu
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

JXStringHistoryMenu::~JXStringHistoryMenu()
{
}

/******************************************************************************
 AddString

	Prepend the given string to the menu and remove outdated entries
	at the bottom.

 ******************************************************************************/

void
JXStringHistoryMenu::AddString
	(
	const JString& str
	)
{
	if (!str.IsEmpty())
	{
		AddItem(str, JString::empty);
	}
}
