/******************************************************************************
 JXStyleTableMenu.cpp

	Menu for changing font styles in a JXStyleTable.  This menu is an action
	menu, so all messages that are broadcast are meaningless to outsiders.

	BASE CLASS = JXStyleMenu

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXStyleTableMenu.h>
#include <JXStyleTable.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXStyleTableMenu::JXStyleTableMenu
	(
	JXStyleTable*		table,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXStyleMenu(kJFalse, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsTable = table;
}

JXStyleTableMenu::JXStyleTableMenu
	(
	JXStyleTable*	table,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXStyleMenu(kJFalse, owner, itemIndex, enclosure)
{
	itsTable = table;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXStyleTableMenu::~JXStyleTableMenu()
{
}

/******************************************************************************
 UpdateMenu (virtual protected)

	We are only active if a cell is being edited.

 ******************************************************************************/

void
JXStyleTableMenu::UpdateMenu()
{
	if (itsTable->IsEditing())
		{
		EnableAll();
		JXStyleMenu::UpdateMenu();
		}
}

/******************************************************************************
 GetFontStyleForMenuUpdate (virtual protected)

 ******************************************************************************/

JFontStyle
JXStyleTableMenu::GetFontStyleForMenuUpdate()
	const
{
	JPoint cell;
	const JBoolean ok = itsTable->GetEditedCell(&cell);
	assert( ok );
	return itsTable->GetCellStyle(cell);
}

/******************************************************************************
 HandleMenuItem (virtual protected)

 ******************************************************************************/

void
JXStyleTableMenu::HandleMenuItem
	(
	const JIndex index
	)
{
	JPoint cell;
	if (itsTable->GetEditedCell(&cell))
		{
		JFontStyle style = itsTable->GetCellStyle(cell);
		UpdateStyle(index, &style);
		itsTable->SetCellStyle(cell, style);
		}
}
