/******************************************************************************
 JXWDMenu.cpp

	Menu listing all registered window directors.  This menu is an action
	menu.  It does not store state information, and all messages that are
	broadcast are meaningless to outsiders.

	BASE CLASS = JXTextMenu

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWDMenu.h>
#include <JXWDManager.h>
#include <JXDisplay.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWDMenu::JXWDMenu
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
	JXWDMenuX();
}

JXWDMenu::JXWDMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	JXWDMenuX();
}

// private

void
JXWDMenu::JXWDMenuX()
{
	itsWDMgr = (GetDisplay())->GetWDManager();
	itsWDMgr->UpdateWDMenu(this);	// so permanent windows can go on toolbar

	ListenTo(this);
	ListenTo(itsWDMgr);

	SetUpdateAction(kDisableNone);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWDMenu::~JXWDMenu()
{
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXWDMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		itsWDMgr->UpdateWDMenu(this);
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		itsWDMgr->HandleWDMenu(this, selection->GetIndex());
		}

	else if (sender == itsWDMgr && message.Is(JXWDManager::kWDMenuNeedsUpdate))
		{
		itsWDMgr->UpdateWDMenu(this);
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}
