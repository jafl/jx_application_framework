/******************************************************************************
 JXDocumentMenu.cpp

	Menu listing all open documents.  This menu is an action menu.  It does
	not store state information, and all messages that are broadcast are
	meaningless to outsiders.

	BASE CLASS = JXTextMenu

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDocumentMenu.h>
#include <JXDocumentManager.h>
#include <JXDocument.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDocumentMenu::JXDocumentMenu
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
	JXTextMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
	JXDocumentMenuX();
}

JXDocumentMenu::JXDocumentMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXDocumentMenuX();
}

// private

void
JXDocumentMenu::JXDocumentMenuX()
{
	itsDocMgr = JXGetDocumentManager();

	ListenTo(this);
	ListenTo(itsDocMgr);

	SetUpdateAction(kDisableNone);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDocumentMenu::~JXDocumentMenu()
{
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXDocumentMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		itsDocMgr->UpdateDocumentMenu(this);
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		JXDocument* doc;
		if (itsDocMgr->GetDocument(selection->GetIndex(), &doc))	// doc might close while menu is open
			{
			doc->Activate();
			}
		}

	else if (sender == itsDocMgr && message.Is(JXDocumentManager::kDocMenuNeedsUpdate))
		{
		itsDocMgr->UpdateDocumentMenu(this);
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}
