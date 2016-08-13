/******************************************************************************
 THXBaseConvMenu.cpp

	Since we have to update our index before anybody can request its new
	value, the correct way to use this object is to listen for the
	BaseChanged message, not the ItemSelected message.  In addition, since
	we are responsible for marking the menu item, clients should ignore
	NeedsUpdate.

	BASE CLASS = JXTextMenu

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "THXBaseConvMenu.h"
#include <JString.h>
#include <jAssert.h>

static const JCharacter* kMenuTitle = "Base:";
static const JCharacter* kMenuStr   = "2 %r | 8 %r | 10 %r | 16 %r";

static const JSize kBaseValue[] =
{
	2, 8, 10, 16
};

const JIndex kDefaultBaseIndex = 3;

// JBroadcaster message types

const JCharacter* THXBaseConvMenu::kBaseChanged = "BaseChanged::THXBaseConvMenu";

/******************************************************************************
 Constructor

 ******************************************************************************/

THXBaseConvMenu::THXBaseConvMenu
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
	JXTextMenu(kMenuTitle, enclosure, hSizing, vSizing, x,y, w,h)
{
	itsItemIndex = kDefaultBaseIndex;

	SetMenuItems(kMenuStr);
	SetUpdateAction(kDisableNone);
	SetToPopupChoice(kJTrue, itsItemIndex);
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

THXBaseConvMenu::~THXBaseConvMenu()
{
}

/******************************************************************************
 GetBase

 ******************************************************************************/

JSize
THXBaseConvMenu::GetBase()
	const
{
	return kBaseValue [ itsItemIndex - 1 ];
}

/******************************************************************************
 SetBase

 ******************************************************************************/

void
THXBaseConvMenu::SetBase
	(
	const JSize base
	)
{
	const JIndex count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (kBaseValue[i-1] == base)
			{
			itsItemIndex = i;
			SetPopupChoice(itsItemIndex);
			Broadcast(BaseChanged(base));
			break;
			}
		}
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
THXBaseConvMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		CheckItem(itsItemIndex);
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsItemIndex = selection->GetIndex();
		Broadcast(BaseChanged(GetBase()));
		}

	else
		{
		JXTextMenu::Receive(sender, message);
		}
}
