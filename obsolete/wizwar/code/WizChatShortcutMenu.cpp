/******************************************************************************
 WizChatShortcutMenu.cpp

	BASE CLASS = JXTextMenu

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "WizChatShortcutMenu.h"
#include "wizGlobals.h"
#include "wwUtil.h"
#include <JXInputField.h>
#include <JString.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

static const JCharacter* kTitlePrefix   = "Player #";
static const JCharacter* kSetupFileName = "chat_messages";

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

WizChatShortcutMenu::WizChatShortcutMenu
	(
	JXInputField*		message,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXTextMenu("", enclosure, hSizing, vSizing, x,y, w,h)
{
	itsMessage = message;

	SetUpdateAction(kDisableNone);
	ListenTo(this);
	ListenTo(WizGetConnectionManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WizChatShortcutMenu::~WizChatShortcutMenu()
{
}

/******************************************************************************
 CheckForShortcut

	If the text in itsMessage matches a shortcut, replace it.

 ******************************************************************************/

void
WizChatShortcutMenu::CheckForShortcut()
{
	JString nmShortcut;
	const JString& text = itsMessage->GetText();

	const JSize count = GetItemCount();
	for (JIndex i=1; i<=count; i++)
		{
		if (GetItemNMShortcut(i, &nmShortcut))
			{
			nmShortcut.TrimWhitespace();
			if (nmShortcut == text)
				{
				itsMessage->SetText(GetItemText(i));
				return;
				}
			}
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
WizChatShortcutMenu::Receive
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
		InitMenu(info->GetPlayerIndex());
		}

	else
		{
		if (sender == this && message.Is(JXMenu::kItemSelected))
			{
			const JXMenu::ItemSelected* info =
				dynamic_cast<const JXMenu::ItemSelected*>(&message);
			assert( info != NULL );
			itsMessage->SetText(GetItemText(info->GetIndex()));
			itsMessage->SetCaretLocation(itsMessage->GetTextLength()+1);
			}

		JXTextMenu::Receive(sender, message);
		}
}

/******************************************************************************
 InitMenu (private)

	Reads { shortcut <std::ws> text \n }.  Blank lines are converted to separators.

 ******************************************************************************/

static const JCharacter delimiters[] = { ' ', '\t', '\n' };
const JSize delimiterCount = sizeof(delimiters)/sizeof(JCharacter);

void
WizChatShortcutMenu::InitMenu
	(
	const JIndex playerIndex
	)
{
	SetTitle((WizGetApplication())->GetPlayerName(), NULL, kJFalse);

	RemoveAllItems();

	JString fileName;
	if (WWGetDataFileName(kSetupFileName, &fileName))
		{
		std::ifstream input(fileName);

		JIndex i = 0;
		JString nmShortcut, text;
		while (1)
			{
			// read one line

			JCharacter c;
			const JBoolean found = JReadUntil(input, delimiterCount, delimiters,
											  &nmShortcut, &c);
			text.Clear();
			if (!found)
				{
				if (!nmShortcut.IsEmpty())
					{
					i++;
					AppendItem(nmShortcut);
					}
				break;
				}
			else if (found && c != '\n')
				{
				text = JReadLine(input);
				text.TrimWhitespace();
				}

			// create the menu item

			if (nmShortcut.IsEmpty() && text.IsEmpty())
				{
				if (i > 0)
					{
					ShowSeparatorAfter(i);
					}
				}
			else if (nmShortcut.IsEmpty())
				{
				i++;
				AppendItem(text);
				}
			else if (text.IsEmpty())
				{
				i++;
				AppendItem(nmShortcut);
				}
			else
				{
				i++;
				AppendItem(text);
				nmShortcut.PrependCharacter(' ');	// avoid accidental match
				SetItemNMShortcut(i, nmShortcut);
				}
			}
		}

	const JSize itemCount = GetItemCount();
	if (itemCount > 0)
		{
		ShowSeparatorAfter(itemCount, kJFalse);
		}
}
