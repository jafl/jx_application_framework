/******************************************************************************
 CBDocumentMenu.cpp

	If Meta is down, sets selected item to active project document
	without activating it.

	BASE CLASS = JXDocumentMenu

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBDocumentMenu.h"
#include "CBProjectDocument.h"
#include "CBSearchDocument.h"
#include "CBCompileDocument.h"
#include "cbGlobals.h"
#include <JXDisplay.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBDocumentMenu::CBDocumentMenu
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
	JXDocumentMenu(title, enclosure, hSizing, vSizing, x,y, w,h)
{
}

CBDocumentMenu::CBDocumentMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXDocumentMenu(owner, itemIndex, enclosure)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBDocumentMenu::~CBDocumentMenu()
{
}

/******************************************************************************
 Receive (protected)

	We absorb the ItemSelected message if the Meta key is down.

 ******************************************************************************/

void
CBDocumentMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kItemSelected) &&
		(GetDisplay()->GetLatestKeyModifiers()).GetState(JXMenu::AdjustNMShortcutModifier(kJXMetaKeyIndex)))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		if (selection->IsFromShortcut())
			{
			JXDocumentMenu::Receive(sender, message);
			}
		else
			{
			JXDocument* doc;
			CBDocumentManager* docMgr = CBGetDocumentManager();
			if (docMgr->GetDocument(selection->GetIndex(), &doc))
				{
				CBProjectDocument* projDoc    = dynamic_cast<CBProjectDocument*>(doc);
				CBSearchDocument* searchDoc   = dynamic_cast<CBSearchDocument*>(doc);
				CBCompileDocument* compileDoc = dynamic_cast<CBCompileDocument*>(doc);
				if (projDoc != NULL)
					{
					docMgr->SetActiveProjectDocument(projDoc);
					}
				else if (searchDoc != NULL)
					{
					docMgr->SetActiveListDocument(searchDoc);
					}
				else if (compileDoc != NULL)
					{
					docMgr->SetActiveListDocument(compileDoc);
					}
				}
			}
		}

	else
		{
		JXDocumentMenu::Receive(sender, message);
		}
}
