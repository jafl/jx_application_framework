/******************************************************************************
 JXTEStyleMenu.cpp

	Menu for changing font styles in a JXTextEditor.  This menu is an action
	menu, so all messages that are broadcast are meaningless to outsiders.

	BASE CLASS = JXStyleMenu

	Copyright (C) 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXTEStyleMenu.h>
#include <JXTextEditor.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTEStyleMenu::JXTEStyleMenu
	(
	JXTextEditor*		te,
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
	itsTE = te;
	SetUpdateAction(kDisableNone);
}

JXTEStyleMenu::JXTEStyleMenu
	(
	JXTextEditor*	te,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXStyleMenu(owner, itemIndex, enclosure)
{
	itsTE = te;
	SetUpdateAction(kDisableNone);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTEStyleMenu::~JXTEStyleMenu()
{
}

/******************************************************************************
 GetFontStyleForMenuUpdate (virtual protected)

 ******************************************************************************/

JFontStyle
JXTEStyleMenu::GetFontStyleForMenuUpdate()
	const
{
	return itsTE->GetCurrentFont().GetStyle();
}

/******************************************************************************
 HandleMenuItem (virtual protected)

	We can't use UpdateStyle() because JTextEditor has to add and
	subtract the styles across style boundaries.

 ******************************************************************************/

void
JXTEStyleMenu::HandleMenuItem
	(
	const JIndex index
	)
{
	const JFontStyle style = GetFontStyleForMenuUpdate();

	if (index == kPlainStyleCmd)
		{
		itsTE->SetCurrentFontStyle(JFontStyle());
		}

	else if (index == kBoldStyleCmd)
		{
		itsTE->SetCurrentFontBold(!style.bold);
		}

	else if (index == kItalicStyleCmd)
		{
		itsTE->SetCurrentFontItalic(!style.italic);
		}

	else if (index == kUnderlineStyleCmd && style.underlineCount != 1)
		{
		itsTE->SetCurrentFontUnderline(1);
		}
	else if (index == kUnderlineStyleCmd)
		{
		itsTE->SetCurrentFontUnderline(0);
		}

	else if (index == kDblUnderlineStyleCmd && style.underlineCount != 2)
		{
		itsTE->SetCurrentFontUnderline(2);
		}
	else if (index == kDblUnderlineStyleCmd)
		{
		itsTE->SetCurrentFontUnderline(0);
		}

	else if (index == kStrikeStyleCmd)
		{
		itsTE->SetCurrentFontStrike(!style.strike);
		}

	else if (index >= kFirstColorCmd)
		{
		itsTE->SetCurrentFontColor(GetSelectedColor());
		}
}
