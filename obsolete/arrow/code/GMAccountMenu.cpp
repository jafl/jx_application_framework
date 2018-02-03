/******************************************************************************
 GMAccountMenu.cc

	BASE CLASS = public JXTextMenu

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#include <GMAccountMenu.h>
#include <GMAccountManager.h>
#include <GMGlobals.h>

#include <jAssert.h>

const JCharacter* GMAccountMenu::kItemsChanged	= "kItemsChanged::GMAccountMenu";

/******************************************************************************
 Constructor

 *****************************************************************************/

GMAccountMenu::GMAccountMenu
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
	ListenTo(this);
	ListenTo(GGetAccountMgr());
	SetUpdateAction(JXMenu::kDisableNone);
	AdjustItems();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMAccountMenu::~GMAccountMenu()
{
}

/******************************************************************************


 ******************************************************************************/

void
GMAccountMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GGetAccountMgr() && message.Is(GMAccountManager::kAccountsChanged))
		{
		AdjustItems();
		Broadcast(ItemsChanged());
		}
	JXTextMenu::Receive(sender, message);
}

/******************************************************************************
 AdjustItems (private)

 ******************************************************************************/

void
GMAccountMenu::AdjustItems()
{
	JPtrArray<JString> accounts(JPtrArrayT::kForgetAll);
	GGetAccountMgr()->GetAccountNames(&accounts);
	RemoveAllItems();
	const JSize count	= accounts.GetElementCount();
	if (count == 0)
		{
		Deactivate();
		}
	else
		{
		Activate();
		}
	for (JIndex i = 1; i <= count; i++)
		{
		AppendItem(*(accounts.GetElement(i)));
		}
	accounts.DeleteAll();
}
