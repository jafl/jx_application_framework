/******************************************************************************
 FileHistoryMenu.cpp

	Menu listing recent documents.  This menu is an action menu.  It does
	not store state information, and all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXFileHistoryMenu

	Copyright © 1998-2023 by John Lindal.

 ******************************************************************************/

#include "FileHistoryMenu.h"
#include "DocumentManager.h"
#include "MenuDocument.h"
#include "globals.h"
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 40;

/******************************************************************************
 Constructor

 ******************************************************************************/

FileHistoryMenu::FileHistoryMenu
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
	JXFileHistoryMenu(kHistoryLength, enclosure, hSizing, vSizing, x,y, w,h)
{
	FileHistoryMenuX();
}

FileHistoryMenu::FileHistoryMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXFileHistoryMenu(kHistoryLength, owner, itemIndex, enclosure)
{
	FileHistoryMenuX();
}

// private

void
FileHistoryMenu::FileHistoryMenuX()
{
	FileHistoryMenu* master =
		GetDocumentManager()->GetFileHistoryMenu();
	if (master != nullptr)
	{
		std::ostringstream data;
		master->WriteSetup(data);
		const std::string s = data.str();
		std::istringstream input(s);
		ReadSetup(input);
	}

	ListenTo(this);
	ListenTo(GetDocumentManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FileHistoryMenu::~FileHistoryMenu()
{
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
FileHistoryMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetDocumentManager() &&
		message.Is(DocumentManager::kAddFileToHistory))
	{
		const auto* info =
			dynamic_cast<const DocumentManager::AddFileToHistory*>(&message);
		assert( info != nullptr );
		AddFile(info->GetFullName());
	}

	else if (sender == this && message.Is(JXMenu::kItemSelected))
	{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		MenuDocument* doc;
		MenuDocument::Create(GetFile(selection->GetIndex()), &doc);
	}

	else
	{
		JXFileHistoryMenu::Receive(sender, message);
	}
}
