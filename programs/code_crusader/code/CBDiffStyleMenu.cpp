/******************************************************************************
 CBDiffStyleMenu.cpp

	Menu for changing font styles for displaying file differences.  This menu
	is an action menu, so all messages that are broadcast are meaningless to
	outsiders.

	BASE CLASS = JXStyleMenu

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBDiffStyleMenu.h"
#include <JXColormap.h>
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
	istream& input
	)
{
	input >> itsStyle.bold >> itsStyle.italic >> itsStyle.strike;
	input >> itsStyle.underlineCount;

	JRGB color;
	input >> color;
	itsStyle.color = (GetColormap())->JColormap::GetColor(color);
}

/******************************************************************************
 WriteStyle

 ******************************************************************************/

void
CBDiffStyleMenu::WriteStyle
	(
	ostream& output
	)
{
	output << ' ' << itsStyle.bold << itsStyle.italic << itsStyle.strike;
	output << ' ' << itsStyle.underlineCount;
	output << ' ' << (GetColormap())->JColormap::GetRGB(itsStyle.color);
}
