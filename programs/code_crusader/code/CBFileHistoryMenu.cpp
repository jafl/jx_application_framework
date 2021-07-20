/******************************************************************************
 CBFileHistoryMenu.cpp

	Menu listing recent documents.  This menu is an action menu.  It does
	not store state information, and all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXFileHistoryMenu

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBFileHistoryMenu.h"
#include "CBProjectDocument.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <JXImage.h>
#include <JXImageCache.h>
#include <sstream>
#include <jAssert.h>

#include "jcc_project_file.xpm"

const JSize kHistoryLength = 40;

/******************************************************************************
 Constructor

 ******************************************************************************/

CBFileHistoryMenu::CBFileHistoryMenu
	(
	const CBDocumentManager::FileHistoryType	type,

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
	JXFileHistoryMenu(kHistoryLength, title, enclosure, hSizing, vSizing, x,y, w,h),
	itsDocType(type)
{
	CBFileHistoryMenuX(type);
}

CBFileHistoryMenu::CBFileHistoryMenu
	(
	const CBDocumentManager::FileHistoryType	type,

	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXFileHistoryMenu(kHistoryLength, owner, itemIndex, enclosure),
	itsDocType(type)
{
	CBFileHistoryMenuX(type);
}

// private

void
CBFileHistoryMenu::CBFileHistoryMenuX
	(
	const CBDocumentManager::FileHistoryType type
	)
{
	if (type == CBDocumentManager::kProjectFileHistory)
		{
		SetDefaultIcon(GetDisplay()->GetImageCache()->GetImage(jcc_project_file), false);
		}

	CBFileHistoryMenu* master =
		CBGetDocumentManager()->GetFileHistoryMenu(itsDocType);
	if (master != nullptr)
		{
		std::ostringstream data;
		master->WriteSetup(data);
		const std::string s = data.str();
		std::istringstream input(s);
		ReadSetup(input);
		}

	ListenTo(this);
	ListenTo(CBGetDocumentManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBFileHistoryMenu::~CBFileHistoryMenu()
{
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
CBFileHistoryMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );

		// need copy because menu will change
		const JString fileName = GetFile(selection->GetIndex());

		bool saveReopen = CBProjectDocument::WillReopenTextFiles();
		if ((GetDisplay()->GetLatestKeyModifiers()).GetState(kJXShiftKeyIndex))
			{
			CBProjectDocument::ShouldReopenTextFiles(!saveReopen);
			}

		CBDocumentManager* docMgr = CBGetDocumentManager();
		if (!(GetDisplay()->GetLatestKeyModifiers()).meta() ||
			(docMgr->CloseProjectDocuments() &&
			 docMgr->CloseTextDocuments()))
			{
			docMgr->OpenSomething(fileName);
			}

		CBProjectDocument::ShouldReopenTextFiles(saveReopen);
		}

	else if (sender == CBGetDocumentManager() &&
			 message.Is(CBDocumentManager::kAddFileToHistory))
		{
		const auto* info =
			dynamic_cast<const CBDocumentManager::AddFileToHistory*>(&message);
		assert( info != nullptr );
		if (itsDocType == info->GetFileHistoryType())
			{
			AddFile(info->GetFullName());
			}
		}

	else
		{
		JXFileHistoryMenu::Receive(sender, message);
		}
}
