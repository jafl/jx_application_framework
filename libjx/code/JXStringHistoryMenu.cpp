/******************************************************************************
 JXStringHistoryMenu.cpp

	Maintains a fixed-length history of strings, with the oldest at the bottom
	of the menu.

	It makes no sense to use this as a PopupChoice menu because it should be
	coupled to a JXInputField.

	BASE CLASS = JXHistoryMenuBase

	Copyright © 1997-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStringHistoryMenu.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStringHistoryMenu::JXStringHistoryMenu
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
	const JCharacter* str
	)
{
	if (!JStringEmpty(str))
		{
		AddItem(str, "");
		}
}
