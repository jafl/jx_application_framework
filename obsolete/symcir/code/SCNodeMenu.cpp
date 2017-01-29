/******************************************************************************
 SCNodeMenu.cpp

	We don't have to worry about an empty menu because there will always
	be ground.

	BASE CLASS = JXTextMenu

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCNodeMenu.h"
#include "SCCircuit.h"
#include <JXWindow.h>
#include <JString.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* SCNodeMenu::kNodeNeedsUpdate = "NodeNeedsUpdate::SCNodeMenu";
const JCharacter* SCNodeMenu::kNodeChanged     = "NodeChanged::SCNodeMenu";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCNodeMenu::SCNodeMenu
	(
	const SCCircuit*	circuit,
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
	SCNodeMenuX(circuit);
}

SCNodeMenu::SCNodeMenu
	(
	const SCCircuit*	circuit,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	SCNodeMenuX(circuit);
}

// private

void
SCNodeMenu::SCNodeMenuX
	(
	const SCCircuit* circuit
	)
{
	itsCircuit = circuit;
	ListenTo(itsCircuit->GetNodeList());

	itsBroadcastNodeChangeFlag = kJTrue;

	BuildMenu();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCNodeMenu::~SCNodeMenu()
{
}

/******************************************************************************
 SetNodeIndex

 ******************************************************************************/

void
SCNodeMenu::SetNodeIndex
	(
	const JIndex node
	)
{
	itsNodeIndex = node;
	SetPopupChoice(itsNodeIndex);
	if (itsBroadcastNodeChangeFlag)
		{
		Broadcast(NodeChanged(itsNodeIndex));
		}
}

/******************************************************************************
 BuildMenu (private)

 ******************************************************************************/

void
SCNodeMenu::BuildMenu()
{
	RemoveAllItems();

	const JSize nodeCount = itsCircuit->GetNodeCount();
	for (JIndex i=1; i<=nodeCount; i++)
		{
		const JString& nodeName = itsCircuit->GetNodeName(i);
		AppendItem(nodeName, kJTrue, kJTrue);
		}

	SetUpdateAction(kDisableNone);

	itsNodeIndex = 1;
	ListenTo(this);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCNodeMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JBroadcaster* nodeList = itsCircuit->GetNodeList();	// ensure that it isn't const

	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		itsBroadcastNodeChangeFlag = kJFalse;
		Broadcast(NodeNeedsUpdate());
		CheckItem(itsNodeIndex);
		itsBroadcastNodeChangeFlag = kJTrue;
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsNodeIndex = selection->GetIndex();
		Broadcast(NodeChanged(itsNodeIndex));
		}

	else if (sender == nodeList)
		{
		if (message.Is(JListT::kElementsInserted))
			{
			const JListT::ElementsInserted* info =
				dynamic_cast<const JListT::ElementsInserted*>(&message);
			assert( info != NULL && info->GetCount() == 1 );
			info->AdjustIndex(&itsNodeIndex);
			}
		else if (message.Is(JListT::kElementsRemoved))
			{
			const JListT::ElementsRemoved* info =
				dynamic_cast<const JListT::ElementsRemoved*>(&message);
			assert( info != NULL && info->GetCount() == 1 );
			if (!info->AdjustIndex(&itsNodeIndex))
				{
				itsNodeIndex = 1;
				}
			}
		else if (message.Is(JListT::kElementMoved))
			{
			const JListT::ElementMoved* info =
				dynamic_cast<const JListT::ElementMoved*>(&message);
			assert( info != NULL );
			info->AdjustIndex(&itsNodeIndex);
			}
		else if (message.Is(JListT::kElementsSwapped))
			{
			const JListT::ElementsSwapped* info =
				dynamic_cast<const JListT::ElementsSwapped*>(&message);
			assert( info != NULL );
			info->AdjustIndex(&itsNodeIndex);
			}
		else if (message.Is(JListT::kSorted))
			{
			assert( 0 );
			}
		// nothing extra required for JListT::kElementChanged

		const JIndex savedNode = itsNodeIndex;
		BuildMenu();
		itsNodeIndex = savedNode;
		SetPopupChoice(itsNodeIndex);
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 SetToPopupChoice

 ******************************************************************************/

void
SCNodeMenu::SetToPopupChoice
	(
	const JBoolean isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsNodeIndex);
}
