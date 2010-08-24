/******************************************************************************
 GMPOPRetrieverMenu.cc

	BASE CLASS = public JXTextMenu

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMPOPRetrieverMenu.h>
#include <GMAccountManager.h>
#include <GMGlobals.h>

#include <jAssert.h>


/******************************************************************************
 Constructor

 *****************************************************************************/

GMPOPRetrieverMenu::GMPOPRetrieverMenu
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
	GMPOPRetrieverMenuX();
}

GMPOPRetrieverMenu::GMPOPRetrieverMenu
	(
	JXMenu*			owner,
	const JIndex	itemIndex,
	JXContainer*	enclosure
	)
	:
	JXTextMenu(owner, itemIndex, enclosure)
{
	GMPOPRetrieverMenuX();
}

void
GMPOPRetrieverMenu::GMPOPRetrieverMenuX()
{
	ListenTo(this);
	ListenTo(GGetAccountMgr());
	SetUpdateAction(JXMenu::kDisableNone);
	AdjustItems();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMPOPRetrieverMenu::~GMPOPRetrieverMenu()
{
}

/******************************************************************************


 ******************************************************************************/

void
GMPOPRetrieverMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GGetAccountMgr() && message.Is(GMAccountManager::kAccountsChanged))
		{
		AdjustItems();
		}
	else if (sender == this && message.Is(JXMenu::kNeedsUpdate))
		{
		if (GGetAccountMgr()->FinishedChecking())
			{
			EnableAll();
			}
		else
			{
			DisableAll();
			}
		}
	else if (sender == this && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		GGetAccountMgr()->CheckAccount(selection->GetIndex());
		}
	JXTextMenu::Receive(sender, message);
}

/******************************************************************************
 AdjustItems (private)

 ******************************************************************************/

void
GMPOPRetrieverMenu::AdjustItems()
{
	JPtrArray<JString> accounts(JPtrArrayT::kForgetAll);
	GGetAccountMgr()->GetPOPAccounts(&accounts);
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
		AppendItem(*(accounts.NthElement(i)), JXMenu::kPlainType, NULL, NULL, *(accounts.NthElement(i)));
		}
	accounts.DeleteAll();
}
