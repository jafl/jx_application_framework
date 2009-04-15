/******************************************************************************
 WizChatDirector.cpp

	BASE CLASS = JXWindowDirector

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "WizChatDirector.h"
#include "WizChatInput.h"
#include "WizChatShortcutMenu.h"
#include "WizChatTargetMenu.h"
#include "WizPlayer.h"
#include "wizGlobals.h"
#include "wizHelpText.h"
#include "wwConstants.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXHelpManager.h>
#include <JXImage.h>
#include <jXActionDefs.h>
#include <JString.h>
#include <jAssert.h>

static const JCharacter* kColorSquareFont = "7x13";
const JSize kColorSquareSize              = 10;
static const JCharacter* kColorSquareStr  = "\x80";

// Actions menu

static const JCharacter* kActionsMenuTitleStr = "Actions";
static const JCharacter* kActionsMenuStr =
	"Quit %k Meta-Q";

enum
{
	kQuitCmd = 1
};

// Prefs menu

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"Double space %b";

enum
{
	kToggleDoubleSpaceCmd = 1
};

// Help menu

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About" 
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kHelpAboutCmd = 1,
	kHelpTOCCmd, kHelpOverviewCmd, kHelpWindowCmd,
	kHelpChangeLogCmd, kHelpCreditsCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

WizChatDirector::WizChatDirector
	(
	JXDirector*	supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	WizChatDirectorX();
	BuildWindow();
}

WizChatDirector::WizChatDirector
	(
	istream&			input,
	const JFileVersion	vers,
	JXDirector*			supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	WizChatDirectorX();

	BuildWindow();
	(GetWindow())->ReadGeometry(input);

	input >> itsDoubleSpaceFlag;
}

// private

void
WizChatDirector::WizChatDirectorX()
{
	itsDoubleSpaceFlag = kJFalse;

	ListenTo(WizGetConnectionManager());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WizChatDirector::~WizChatDirector()
{
}

/******************************************************************************
 WritePrefs

 ******************************************************************************/

void
WizChatDirector::WritePrefs
	(
	ostream& output
	)
	const
{
	(GetWindow())->WriteGeometry(output);

	output << ' ' << itsDoubleSpaceFlag;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include <jx_un_message.xpm>

void
WizChatDirector::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 600,250, "");
    assert( window != NULL );
    SetWindow(window);

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 600,30);
    assert( menuBar != NULL );

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 600,180);
    assert( scrollbarSet != NULL );

    itsChatInput =
        new WizChatInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 90,210, 510,40);
    assert( itsChatInput != NULL );

    itsChatShortcutMenu =
        new WizChatShortcutMenu(itsChatInput, window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,210, 90,20);
    assert( itsChatShortcutMenu != NULL );

    itsChatTargetMenu =
        new WizChatTargetMenu(itsChatInput, itsChatShortcutMenu, window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 0,230, 90,20);
    assert( itsChatTargetMenu != NULL );

// end JXLayout

	window->SetTitle("Chat");
	window->SetMinSize(150, 150);
	window->SetWMClass(WizGetWMClassInstance(), WizGetChatWindowClass());

	JXDisplay* display = GetDisplay();
	JXImage* icon      = new JXImage(display, display->GetColormap(), jx_un_message);
	assert( icon != NULL );
	window->SetIcon(icon);

	itsMessageDisplay =
		new JXStaticText("", kJTrue, kJTrue,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic,
						 0,0, 100,100);
	assert( itsMessageDisplay != NULL );
	itsMessageDisplay->FitToEnclosure();
	itsMessageDisplay->SetBackColor(itsMessageDisplay->GetFocusColor());

	itsChatInput->SetShortcutMenu(itsChatShortcutMenu);
	ListenTo(itsChatTargetMenu);

	itsActionsMenu = menuBar->AppendTextMenu(kActionsMenuTitleStr);
	itsActionsMenu->SetMenuItems(kActionsMenuStr);
	itsActionsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsActionsMenu);

	itsMessageDisplay->AppendEditMenu(menuBar);
	itsChatInput->ShareEditMenu(itsMessageDisplay);

	itsPrefsMenu = menuBar->AppendTextMenu(kPrefsMenuTitleStr);
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr);
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPrefsMenu);

	itsHelpMenu = menuBar->AppendTextMenu(kHelpMenuTitleStr);
	itsHelpMenu->SetMenuItems(kHelpMenuStr);
	itsHelpMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsHelpMenu);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
WizChatDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	Wiz2War* connMgr = WizGetConnectionManager();

	if (sender == connMgr && message.Is(Wiz2War::kChatMessage))
		{
		const Wiz2War::ChatMessage* info =
			dynamic_cast(const Wiz2War::ChatMessage*, &message);
		assert( info != NULL );
		AppendMessage(info->GetSenderIndex(), info->GetMessage());
		(GetWindow())->Deiconify();
		}

	else if (sender == connMgr && message.Is(Wiz2War::kPlayerJoined))
		{
		const Wiz2War::PlayerJoined* info =
			dynamic_cast(const Wiz2War::PlayerJoined*, &message);
		assert( info != NULL );
		NotifyPlayerJoined(*info);
		}
	else if (sender == connMgr && message.Is(Wiz2War::kPlayerLeft))
		{
		const Wiz2War::PlayerLeft* info =
			dynamic_cast(const Wiz2War::PlayerLeft*, &message);
		assert( info != NULL );
		NotifyPlayerLeft(info->GetIndex(), info->TimedOut());
		}

	else if (sender == itsChatTargetMenu &&
			 message.Is(WizChatTargetMenu::kSendChatMessage))
		{
		const WizChatTargetMenu::SendChatMessage* info =
			dynamic_cast(const WizChatTargetMenu::SendChatMessage*, &message);
		assert( info != NULL );
		if (info->GetReceiverIndex() != kWWAllPlayersIndex)
			{
			EchoMessage(info->GetReceiverIndex(), info->GetMessage());
			}
		connMgr->SendChatMessage(info->GetReceiverIndex(), info->GetMessage());
		}

	else if (sender == itsActionsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateActionsMenu();
		}
	else if (sender == itsActionsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleActionsMenu(selection->GetIndex());
		}

	else if (sender == itsPrefsMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdatePrefsMenu();
		}
	else if (sender == itsPrefsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandlePrefsMenu(selection->GetIndex());
		}

	else if (sender == itsHelpMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateHelpMenu();
		}
	else if (sender == itsHelpMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleHelpMenu(selection->GetIndex());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 NotifyPlayerJoined (private)

 ******************************************************************************/

void
WizChatDirector::NotifyPlayerJoined
	(
	const Wiz2War::PlayerJoined& info
	)
{
	const WizPlayer& player = info.GetPlayer();

	JString msg;
	if (info.GetIndex() == (WizGetConnectionManager())->GetPlayerIndex())
		{
		msg = "You are player #";
		}
	else
		{
		msg = player.GetName();
		if (!(player.GetEmail()).IsEmpty())
			{
			msg += " <";
			msg += player.GetEmail();
			msg += ">";
			}

		if (info.IsNew())
			{
			msg += " has joined the game as player #";
			}
		else
			{
			msg += " is player #";
			}
		}
	msg += JString(info.GetIndex(), 0);
	msg += ".";

	AppendMessage(kWWAllPlayersIndex, msg, player.GetColor());

	// We can't do this in BuildWindow() because the menus adjust when the
	// connection is accepted.

	const JCoordinate width = JMax(itsChatShortcutMenu->GetFrameWidth(),
								   itsChatTargetMenu->GetFrameWidth());
	const JRect frame = itsChatInput->GetFrame();
	itsChatInput->Place(width, frame.top);
	itsChatInput->SetSize((GetWindow())->GetFrameWidth() - width, frame.height());
}

/******************************************************************************
 NotifyPlayerLeft (private)

 ******************************************************************************/

void
WizChatDirector::NotifyPlayerLeft
	(
	const JIndex	index,
	const JBoolean	timeout
	)
{
	const WizPlayer* player = (WizGetConnectionManager())->GetPlayer(index);

	JString msg = player->GetName();
	if (timeout)
		{
		msg += "'s network connection died.";
		}
	else
		{
		msg += " has left the game.";
		}

	AppendMessage(kWWAllPlayersIndex, msg, player->GetColor());
}

/******************************************************************************
 EchoMessage (private)

 ******************************************************************************/

void
WizChatDirector::EchoMessage
	(
	const JIndex		receiverIndex,
	const JCharacter*	msg
	)
{
	Wiz2War* connMgr = WizGetConnectionManager();

	JString s = msg;
	s += " [to ";
	s += (connMgr->GetPlayer(receiverIndex))->GetName();
	s += "]";
	AppendMessage(connMgr->GetPlayerIndex(), s);
}

/******************************************************************************
 AppendMessage (private)

	senderIndex == kWWAllPlayersIndex => don't mention who sent it

	color != 0 => color the last character of msg

 ******************************************************************************/

void
WizChatDirector::AppendMessage
	(
	const JIndex		senderIndex,
	const JCharacter*	msg,
	const JColorIndex	color
	)
{
	Wiz2War* connMgr = WizGetConnectionManager();

	itsMessageDisplay->SetCaretLocation(itsMessageDisplay->GetTextLength()+1);

	const JBoolean self = JI2B(senderIndex == connMgr->GetPlayerIndex());
	if (senderIndex != kWWAllPlayersIndex)
		{
		if (self)
			{
			itsMessageDisplay->Paste("(");
			}

		const WizPlayer* player = connMgr->GetPlayer(senderIndex);

		itsMessageDisplay->Paste(kColorSquareStr);
		itsMessageDisplay->
			JTextEditor::SetFont(itsMessageDisplay->GetTextLength(),
								 itsMessageDisplay->GetTextLength(),
								 kColorSquareFont, kColorSquareSize,
								 JFontStyle(player->GetColor()), kJFalse);

		JString s = " ";
		s += player->GetName();
		if (msg[0] == '"')
			{
			s += " says, ";
			}
		else
			{
			s+= " ";
			}
		itsMessageDisplay->Paste(s);
		}
	else if (color > 0)
		{
		itsMessageDisplay->Paste(kColorSquareStr);
		itsMessageDisplay->
			JTextEditor::SetFont(itsMessageDisplay->GetTextLength(),
								 itsMessageDisplay->GetTextLength(),
								 kColorSquareFont, kColorSquareSize,
								 JFontStyle(color), kJFalse);
		itsMessageDisplay->Paste(" ");
		}

	itsMessageDisplay->Paste(msg);

	if (self)
		{
		itsMessageDisplay->Paste(")");
		}

	if (itsDoubleSpaceFlag)
		{
		itsMessageDisplay->Paste("\n\n");
		}
	else
		{
		itsMessageDisplay->Paste("\n");
		}

	itsMessageDisplay->ClearUndo();
}

/******************************************************************************
 UpdateActionsMenu (private)

 ******************************************************************************/

void
WizChatDirector::UpdateActionsMenu()
{
}

/******************************************************************************
 HandleActionsMenu (private)

 ******************************************************************************/

void
WizChatDirector::HandleActionsMenu
	(
	const JIndex index
	)
{
	if (index == kQuitCmd)
		{
		(WizGetApplication())->Quit();
		}
}

/******************************************************************************
 UpdatePrefsMenu (private)

 ******************************************************************************/

void
WizChatDirector::UpdatePrefsMenu()
{
	if (itsDoubleSpaceFlag)
		{
		itsPrefsMenu->CheckItem(kToggleDoubleSpaceCmd);
		}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
WizChatDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kToggleDoubleSpaceCmd)
		{
		itsDoubleSpaceFlag = !itsDoubleSpaceFlag;
		}
}

/******************************************************************************
 UpdateHelpMenu (private)

 ******************************************************************************/

void
WizChatDirector::UpdateHelpMenu()
{
}

/******************************************************************************
 HandleHelpMenu (private)

 ******************************************************************************/

void
WizChatDirector::HandleHelpMenu
	(
	const JIndex index
	)
{
	if (index == kHelpAboutCmd)
		{
//		(WizGetApplication())->DisplayAbout();
		}

	else if (index == kHelpTOCCmd)
		{
		(JXGetHelpManager())->ShowSection(kWizTOCHelpName);
		}
	else if (index == kHelpOverviewCmd)
		{
		(JXGetHelpManager())->ShowSection(kWizOverviewHelpName);
		}
	else if (index == kHelpWindowCmd)
		{
		(JXGetHelpManager())->ShowSection(kWizChatHelpName);
		}

	else if (index == kHelpChangeLogCmd)
		{
		(JXGetHelpManager())->ShowSection(kWizChangeLogName);
		}
	else if (index == kHelpCreditsCmd)
		{
		(JXGetHelpManager())->ShowSection(kWizCreditsName);
		}
}
