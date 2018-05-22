/******************************************************************************
 CBDiffStyleMenu.cpp

	Menu for changing font styles for displaying file differences.  This menu
	is an action menu, so all messages that are broadcast are meaningless to
	outsiders.

	BASE CLASS = JXStyleMenu

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBDiffStyleMenu.h"
#include <JXColorManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDiffStyleMenu::CBDiffStyleMenu
	(
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
	JXStyleMenu(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetTitle(title, NULL, kJFalse);
	SetUpdateAction(kDisableNone);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDiffStyleMenu::~CBDiffStyleMenu()
{
}

/******************************************************************************
 GetFontStyleForMenuUpdate (virtual protected)

 ******************************************************************************/

JFontStyle
CBDiffStyleMenu::GetFontStyleForMenuUpdate()
	const
{
	return itsStyle;
}

/******************************************************************************
 HandleMenuItem (virtual protected)

 ******************************************************************************/

void
CBDiffStyleMenu::HandleMenuItem
	(
	const JIndex index
	)
{
	UpdateStyle(index, &itsStyle);
}

/******************************************************************************
 ReadStyle

 ******************************************************************************/

void
CBDiffStyleMenu::ReadStyle
	(
	std::istream& input
	)
{
	input >> itsStyle.bold >> itsStyle.italic >> itsStyle.strike;
	input >> itsStyle.underlineCount;

	JRGB color;
	input >> color;
	itsStyle.color = GetColormap()->JXColorManager::GetColor(color);
}

/******************************************************************************
 WriteStyle

 ******************************************************************************/

void
CBDiffStyleMenu::WriteStyle
	(
	std::ostream& output
	)
{
	output << ' ' << itsStyle.bold << itsStyle.italic << itsStyle.strike;
	output << ' ' << itsStyle.underlineCount;
	output << ' ' << GetColormap()->JXColorManager::GetRGB(itsStyle.color);
}
