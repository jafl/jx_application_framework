/******************************************************************************
 SCComponentMenu.cpp

	As soon as we have a way of dynamically changing the circuit, this class
	needs to be stress tested to check that Receive() works and that nothing
	fails when there are no components in the menu.

	BASE CLASS = JXTextMenu

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCComponentMenu.h"
#include "SCCircuit.h"
#include "SCComponent.h"
#include <JXWindow.h>
#include <JString.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* SCComponentMenu::kComponentNeedsUpdate = "ComponentNeedsUpdate::SCComponentMenu";
const JCharacter* SCComponentMenu::kComponentChanged     = "ComponentChanged::SCComponentMenu";

/******************************************************************************
 Constructor

 ******************************************************************************/

SCComponentMenu::SCComponentMenu
	(
	const SCCircuit*	circuit,
	SCComponentFilter*	filter,
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
	SCComponentMenuX(circuit, filter);
}

SCComponentMenu::SCComponentMenu
	(
	const SCCircuit*	circuit,
	SCComponentFilter*	filter,
	JXMenu*				owner,
	const JIndex		itemIndex,
	JXContainer*		enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	SCComponentMenuX(circuit, filter);
}

// private

void
SCComponentMenu::SCComponentMenuX
	(
	const SCCircuit*	circuit,
	SCComponentFilter*	filter
	)
{
	itsCircuit = circuit;
	ListenTo(itsCircuit->GetComponentList());

	itsFilter = filter;

	itsCompList = new JArray<JIndex>;
	assert( itsCompList != NULL );

	itsBroadcastCompChangeFlag = kJTrue;

	BuildCompList();
	BuildMenu();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCComponentMenu::~SCComponentMenu()
{
	delete itsCompList;
}

/******************************************************************************
 SetCompIndex

	Returns kJTrue if the given index was valid.

 ******************************************************************************/

JBoolean
SCComponentMenu::SetCompIndex
	(
	const JIndex compIndex
	)
{
	JIndex menuIndex;
	if (CompIndexToMenuIndex(compIndex, &menuIndex))
		{
		itsMenuIndex = menuIndex;
		SetPopupChoice(itsMenuIndex);
		if (itsBroadcastCompChangeFlag)
			{
			Broadcast(ComponentChanged(compIndex));
			}
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 BuildCompList (private)

	Build a list of the valid components.

 ******************************************************************************/

void
SCComponentMenu::BuildCompList()
{
	itsCompList->RemoveAll();

	const JSize compCount = itsCircuit->GetComponentCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		const SCComponent* comp = itsCircuit->GetComponent(i);
		if (itsFilter(*comp))
			{
			itsCompList->AppendElement(i);
			}
		}

	if (itsCompList->IsEmpty())
		{
		Deactivate();
		}
	else
		{
		Activate();
		}
}

/******************************************************************************
 MenuIndexToCompIndex (private)

	Converts an index into the menu into an index in the component list.

 ******************************************************************************/

JBoolean
SCComponentMenu::MenuIndexToCompIndex
	(
	const JIndex	menuIndex,
	JIndex*			compIndex
	)
	const
{
	if (itsCompList->IndexValid(menuIndex))
		{
		*compIndex = itsCompList->GetElement(menuIndex);
		return kJTrue;
		}
	else
		{
		*compIndex = 0;
		return kJFalse;
		}
}

/******************************************************************************
 CompIndexToMenuIndex (private)

	Converts an index into the component list into an index in the menu.

 ******************************************************************************/

JBoolean
SCComponentMenu::CompIndexToMenuIndex
	(
	const JIndex	compIndex,
	JIndex*			menuIndex
	)
	const
{
	const JSize compCount = itsCompList->GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		if (compIndex == itsCompList->GetElement(i))
			{
			*menuIndex = i;
			return kJTrue;
			}
		}

	*menuIndex = 0;
	return kJFalse;
}

/******************************************************************************
 CompIndexValid

	Returns kJTrue if the specified component is in our menu.

 ******************************************************************************/

JBoolean
SCComponentMenu::CompIndexValid
	(
	const JIndex compIndex
	)
	const
{
	if (itsCircuit->CompIndexValid(compIndex))
		{
		const SCComponent* comp = itsCircuit->GetComponent(compIndex);
		return itsFilter(*comp);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 BuildMenu (private)

 ******************************************************************************/

void
SCComponentMenu::BuildMenu()
{
	RemoveAllItems();

	const JSize compCount = itsCompList->GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		const JIndex compIndex  = itsCompList->GetElement(i);
		const SCComponent* comp = itsCircuit->GetComponent(compIndex);
		const JString& compName = comp->GetName();
		AppendItem(compName, kJTrue, kJTrue);
		}

	SetUpdateAction(kDisableNone);

	itsMenuIndex = 1;
	ListenTo(this);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
SCComponentMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JBroadcaster* compList = itsCircuit->GetComponentList();	// ensure that it isn't const

	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		itsBroadcastCompChangeFlag = kJFalse;
		Broadcast(ComponentNeedsUpdate());
		CheckItem(itsMenuIndex);
		itsBroadcastCompChangeFlag = kJTrue;
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsMenuIndex = selection->GetIndex();

		JIndex compIndex;
		const JBoolean ok = MenuIndexToCompIndex(itsMenuIndex, &compIndex);
		assert( ok );
		Broadcast(ComponentChanged(compIndex));
		}

	else if (sender == compList)
		{
		JIndex compIndex;
		const JBoolean ok = MenuIndexToCompIndex(itsMenuIndex, &compIndex);
		BuildCompList();

		if (!ok)
			{
			itsMenuIndex = 1;
			}
		else if (message.Is(JOrderedSetT::kElementsInserted))
			{
			const JOrderedSetT::ElementsInserted* info =
				dynamic_cast<const JOrderedSetT::ElementsInserted*>(&message);
			assert( info != NULL && info->GetCount() == 1 );
			info->AdjustIndex(&compIndex);
			const JBoolean ok = CompIndexToMenuIndex(compIndex, &itsMenuIndex);
			assert( ok );
			}
		else if (message.Is(JOrderedSetT::kElementsRemoved))
			{
			const JOrderedSetT::ElementsRemoved* info =
				dynamic_cast<const JOrderedSetT::ElementsRemoved*>(&message);
			assert( info != NULL && info->GetCount() == 1 );
			if (info->AdjustIndex(&compIndex))
				{
				const JBoolean ok = CompIndexToMenuIndex(compIndex, &itsMenuIndex);
				assert( ok );
				}
			else
				{
				itsMenuIndex = 1;
				}
			}
		else if (message.Is(JOrderedSetT::kElementMoved))
			{
			const JOrderedSetT::ElementMoved* info =
				dynamic_cast<const JOrderedSetT::ElementMoved*>(&message);
			assert( info != NULL );
			info->AdjustIndex(&compIndex);
			const JBoolean ok = CompIndexToMenuIndex(compIndex, &itsMenuIndex);
			assert( ok );
			}
		else if (message.Is(JOrderedSetT::kElementsSwapped))
			{
			const JOrderedSetT::ElementsSwapped* info =
				dynamic_cast<const JOrderedSetT::ElementsSwapped*>(&message);
			assert( info != NULL );
			info->AdjustIndex(&compIndex);
			const JBoolean ok = CompIndexToMenuIndex(compIndex, &itsMenuIndex);
			assert( ok );
			}
		else if (message.Is(JOrderedSetT::kSorted))
			{
			assert( 0 );
			}
		// nothing extra required for JOrderedSetT::kElementChanged

		const JIndex savedIndex = itsMenuIndex;
		BuildMenu();
		itsMenuIndex = savedIndex;
		if (itsCompList->IndexValid(itsMenuIndex))
			{
			SetPopupChoice(itsMenuIndex);
			}
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
SCComponentMenu::SetToPopupChoice
	(
	const JBoolean isPopup
	)
{
	JXTextMenu::SetToPopupChoice(isPopup, itsMenuIndex);
}

/******************************************************************************
 SCACSourceFilter

 ******************************************************************************/

JBoolean
SCACSourceFilter
	(
	const SCComponent& comp
	)
{
	return JConvertToBoolean( comp.CastToSCACSource() != NULL );
}

/******************************************************************************
 SCDepSourceFilter

 ******************************************************************************/

JBoolean
SCDepSourceFilter
	(
	const SCComponent& comp
	)
{
	return JConvertToBoolean( comp.CastToSCDepSource() != NULL );
}

/******************************************************************************
 SCPassiveLinearFilter

 ******************************************************************************/

JBoolean
SCPassiveLinearFilter
	(
	const SCComponent& comp
	)
{
	return JConvertToBoolean( comp.CastToSCPassiveLinearComp() != NULL );
}
