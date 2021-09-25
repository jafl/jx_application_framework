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
#include <JXDisplay.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_executable_small.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFSCommandHistoryMenu::JXFSCommandHistoryMenu
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
	SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jx_executable_small), false);
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
	bool*						singleFile
	)
	const
{
	assert( message.Is(JXMenu::kItemSelected) );

	const auto* selection = dynamic_cast<const JXMenu::ItemSelected*>(&message);
	assert( selection != nullptr );

	return GetCommand(selection->GetIndex(), type, singleFile);
}

const JString&
JXFSCommandHistoryMenu::GetCommand
	(
	const JIndex				index,
	JFSBinding::CommandType*	type,
	bool*						singleFile
	)
	const
{
	JString typeStr;
	if (!GetItemNMShortcut(index, &typeStr))
	{
		*type       = JFSBinding::kRunPlain;
		*singleFile = false;
	}
	else if (typeStr == JGetString("RunInShell::JXFSCommandHistoryMenu"))
	{
		*type       = JFSBinding::kRunInShell;
		*singleFile = false;
	}
	else if (typeStr == JGetString("RunInShellSingle::JXFSCommandHistoryMenu"))
	{
		*type       = JFSBinding::kRunInShell;
		*singleFile = true;
	}
	else if (typeStr == JGetString("RunInWindow::JXFSCommandHistoryMenu"))
	{
		*type       = JFSBinding::kRunInWindow;
		*singleFile = false;
	}
	else if (typeStr == JGetString("RunInWindowSingle::JXFSCommandHistoryMenu"))
	{
		*type       = JFSBinding::kRunInWindow;
		*singleFile = true;
	}
	else
	{
		assert( typeStr == JGetString("RunSingle::JXFSCommandHistoryMenu") );

		*type       = JFSBinding::kRunPlain;
		*singleFile = true;
	}

	return JXTextMenu::GetItemText(index);
}

/******************************************************************************
 AddCommand

 ******************************************************************************/

void
JXFSCommandHistoryMenu::AddCommand
	(
	const JString&	cmd,
	const bool		inShell,
	const bool		inWindow,
	const bool		singleFile
	)
{
	if (cmd.IsEmpty())
	{
		return;
	}

	const JUtf8Byte* type = nullptr;
	if (inWindow && singleFile)
	{
		type = "RunInWindowSingle::JXFSCommandHistoryMenu";
	}
	else if (inWindow)
	{
		type = "RunInWindow::JXFSCommandHistoryMenu";
	}
	else if (inShell && singleFile)
	{
		type = "RunInShellSingle::JXFSCommandHistoryMenu";
	}
	else if (inShell)
	{
		type = "RunInShell::JXFSCommandHistoryMenu";
	}
	else if (singleFile)
	{
		type = "RunSingle::JXFSCommandHistoryMenu";
	}

	AddItem(cmd, type == nullptr ? JString::empty : JGetString(type));
}
