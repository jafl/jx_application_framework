/******************************************************************************
 WizChatInput.cpp

	BASE CLASS = JXInputField

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "WizChatInput.h"
#include "WizChatShortcutMenu.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WizChatInput::WizChatInput
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(kJTrue, kJFalse, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsShortcutMenu = NULL;
	WantInput(kJTrue, kJTrue);	// tab expands shortcut
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WizChatInput::~WizChatInput()
{
}

/******************************************************************************
 SetShortcutMenu

	We are created before the menus.

 ******************************************************************************/

void
WizChatInput::SetShortcutMenu
	(
	WizChatShortcutMenu* menu
	)
{
	itsShortcutMenu = menu;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
WizChatInput::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == '\t')
		{
		itsShortcutMenu->CheckForShortcut();
		SetCaretLocation(GetTextLength()+1);
		}
	else
		{
		JXInputField::HandleKeyPress(key, modifiers);
		}
}
