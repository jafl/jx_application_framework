/******************************************************************************
 JXFSCommandHistoryMenu.cpp

	Maintains a fixed-length history of commands, with the oldest at the
	bottom of the menu.

	The action type (silent, shell, window) is displayed as the shortcut,
	and the user's command is displayed as the menu item text.  Since the
	type is encoded, always use GetCommand() to get the command.

	The default icon for each file is the executable.

	BASE CLASS = JXHistoryMenuBase

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "JXFSCommandHistoryMenu.h"
#include <JXImage.h>
#include <JString.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

#include <jx_executable_small.xpm>

static const JCharacter* kRunSingleStr         = " (one at a time) ";
static const JCharacter* kRunInShellStr        = " (shell) ";
static const JCharacter* kRunInWindowStr       = " (window) ";
static const JCharacter* kRunInShellSingleStr  = " (shell, one at a time) ";
static const JCharacter* kRunInWindowSingleStr = " (window, one at a time) ";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSCommandHistoryMenu::JXFSCommandHistoryMenu
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
	JFSCommandHistoryMenuX();
}

JXFSCommandHistoryMenu::JXFSCommandHistoryMenu
	(
	const JSize		historyLength,
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXHistoryMenuBase(historyLength, owner, itemIndex, enclosure)
{
	JFSCommandHistoryMenuX();
}

// private

void
JXFSCommandHistoryMenu::JFSCommandHistoryMenuX()
{
	JXImage* icon = jnew JXImage(GetDisplay(), jx_executable_small);
	assert( icon != NULL );
	SetDefaultIcon(icon, kJTrue);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFSCommandHistoryMenu::~JXFSCommandHistoryMenu()
{
}

/******************************************************************************
 GetCommand

 ******************************************************************************/

const JString&
JXFSCommandHistoryMenu::GetCommand
	(
	const Message&				message,
	JFSBinding::CommandType*	type,
	JBoolean*					singleFile
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const JXMenu::ItemSelected* selection =
		dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != NULL );

	return GetCommand(selection->GetIndex(), type, singleFile);
}

const JString&
JXFSCommandHistoryMenu::GetCommand
	(
	const JIndex				index,
	JFSBinding::CommandType*	type,
	JBoolean*					singleFile
	)
	const
{
	JString typeStr;
	if (!GetItemNMShortcut(index, &typeStr))
		{
		*type       = JFSBinding::kRunPlain;
		*singleFile = kJFalse;
		}
	else if (typeStr == kRunInShellStr)
		{
		*type       = JFSBinding::kRunInShell;
		*singleFile = kJFalse;
		}
	else if (typeStr == kRunInShellSingleStr)
		{
		*type       = JFSBinding::kRunInShell;
		*singleFile = kJTrue;
		}
	else if (typeStr == kRunInWindowStr)
		{
		*type       = JFSBinding::kRunInWindow;
		*singleFile = kJFalse;
		}
	else if (typeStr == kRunInWindowSingleStr)
		{
		*type       = JFSBinding::kRunInWindow;
		*singleFile = kJTrue;
		}
	else
		{
		assert( typeStr == kRunSingleStr );

		*type       = JFSBinding::kRunPlain;
		*singleFile = kJTrue;
		}

	return JXTextMenu::GetItemText(index);
}

/******************************************************************************
 AddCommand

 ******************************************************************************/

void
JXFSCommandHistoryMenu::AddCommand
	(
	const JCharacter*	cmd,
	const JBoolean		inShell,
	const JBoolean		inWindow,
	const JBoolean		singleFile
	)
{
	if (JString::IsEmpty(cmd))
		{
		return;
		}

	const JCharacter* type = "";
	if (inWindow && singleFile)
		{
		type = kRunInWindowSingleStr;
		}
	else if (inWindow)
		{
		type = kRunInWindowStr;
		}
	else if (inShell && singleFile)
		{
		type = kRunInShellSingleStr;
		}
	else if (inShell)
		{
		type = kRunInShellStr;
		}
	else if (singleFile)
		{
		type = kRunSingleStr;
		}

	AddItem(cmd, type);
}
