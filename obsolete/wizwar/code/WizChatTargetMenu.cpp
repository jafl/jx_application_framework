/******************************************************************************
 WizChatTargetMenu.cpp

	BASE CLASS = JXTextMenu

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "WizChatTargetMenu.h"
#include "WizChatShortcutMenu.h"
#include "WizPlayer.h"
#include "wizGlobals.h"
#include "wwConstants.h"
#include <JXInputField.h>
#include <JString.h>
#include <jAssert.h>

static const JCharacter* kTitle = "Send to";

static const JCharacter* kAllPlayersText     = "All";
static const JCharacter* kAllPlayersShortcut = "Meta-0";

static const JCharacter* kEmptySlotPrefix = "Player ";

// JBroadcaster message types

const JCharacter* WizChatTargetMenu::kSendChatMessage = "SendChatMessage::WizChatTargetMenu";

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

WizChatTargetMenu::WizChatTargetMenu
	(
	JXInputField*			message,
	WizChatShortcutMenu*	msgMenu,
	JXContainer*			enclosure,
	const HSizingOption		hSizing,
	const VSizingOption		vSizing,
	const JCoordinate		x,
	const JCoordinate		y,
	const JCoordinate		w,
	const JCoordinate		h
	)
	:
	JXTextMenu(kTitle, enclosure, hSizing, vSizing, x,y, w,h)
{
	assert( kWWAllPlayersIndex == 0 );

	itsMessage     = message;
	itsMessageMenu = msgMenu;

	SetUpdateAction(kDisableNone);
	ListenTo(this);
	ListenTo(WizGetConnectionManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WizChatTargetMenu::~WizChatTargetMenu()
{
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
WizChatTargetMenu::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	Wiz2War* connMgr = WizGetConnectionManager();

	if (sender == connMgr && message.Is(Wiz2War::kConnectionAccepted))
		{
		const Wiz2War::ConnectionAccepted* info =
			dynamic_cast<const Wiz2War::ConnectionAccepted*>(&message);
		assert( info != NULL );
		InitMenu(info->GetMaxPlayerCount());
		}

	else if (sender == connMgr && message.Is(Wiz2War::kPlayerJoined))
		{
		const Wiz2War::PlayerJoined* info =
			dynamic_cast<const Wiz2War::PlayerJoined*>(&message);
		assert( info != NULL );
		if (info->GetIndex() != connMgr->GetPlayerIndex())
			{
			EnableItem(info->GetIndex()+1);
			}
		SetItemText(info->GetIndex()+1, (info->GetPlayer()).GetName());
		}

	else if (sender == connMgr && message.Is(Wiz2War::kPlayerLeft))
		{
		const Wiz2War::PlayerLeft* info =
			dynamic_cast<const Wiz2War::PlayerLeft*>(&message);
		assert( info != NULL );
		DisableItem(info->GetIndex()+1);
		const JString text = kEmptySlotPrefix + JString(info->GetIndex(), JString::kBase10);
		SetItemText(info->GetIndex()+1, text);
		}

	else
		{
		if (sender == this && message.Is(JXMenu::kItemSelected))
			{
			const JXMenu::ItemSelected* info =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( info != NULL );
			itsMessageMenu->CheckForShortcut();
			Broadcast(SendChatMessage(info->GetIndex()-1, itsMessage->GetText()));
			itsMessage->SelectAll();
			}

		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 InitMenu (private)

 ******************************************************************************/

void
WizChatTargetMenu::InitMenu
	(
	const JSize count
	)
{
	RemoveAllItems();

	AppendItem(kAllPlayersText);
	SetItemNMShortcut(1, kAllPlayersShortcut);

	JString indexStr, text, nmShortcut;
	for (JIndex i=1; i<=count; i++)
		{
		indexStr = JString(i, JString::kBase10);
		text     = kEmptySlotPrefix + indexStr;
		AppendItem(text);
		if (i <= 9)
			{
			nmShortcut = "Meta-" + indexStr;
			SetItemNMShortcut(i+1, nmShortcut);
			}
		}

	DisableAll();
	EnableItem(1);
}
