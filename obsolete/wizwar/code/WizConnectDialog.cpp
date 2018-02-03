/******************************************************************************
 WizConnectDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "WizConnectDialog.h"
#include "wizHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXHelpManager.h>
#include <jXGlobals.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WizConnectDialog::WizConnectDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	serverAddr,
	const JCharacter*	playerName
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(serverAddr, playerName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WizConnectDialog::~WizConnectDialog()
{
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
WizConnectDialog::Activate()
{
	JXDialogDirector::Activate();
	if (IsActive() && !(itsServerAddr->GetText()).IsEmpty())
		{
		itsPlayerName->Focus();
		}
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
WizConnectDialog::BuildWindow
	(
	const JCharacter* serverAddr,
	const JCharacter* playerName
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 370,110, "");
	assert( window != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::WizConnectDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 260,85, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::WizConnectDialog::shortcuts::JXLayout"));

	JXTextButton* quitButton =
		new JXTextButton(JGetString("quitButton::WizConnectDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,85, 60,20);
	assert( quitButton != NULL );

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::WizConnectDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 155,85, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::WizConnectDialog::shortcuts::JXLayout"));

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::WizConnectDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 100,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::WizConnectDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 100,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsServerAddr =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 230,20);
	assert( itsServerAddr != NULL );

	itsPlayerName =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,40, 230,20);
	assert( itsPlayerName != NULL );

// end JXLayout

	window->SetTitle("Connect to WizWar server");
	SetButtons(okButton, quitButton);

	ListenTo(itsHelpButton);

	itsServerAddr->SetText(serverAddr);
	itsServerAddr->SetIsRequired();

	itsPlayerName->SetText(playerName);
	itsPlayerName->SetIsRequired();
}

/******************************************************************************
 GetConnectInfo

 ******************************************************************************/

void
WizConnectDialog::GetConnectInfo
	(
	JString* serverAddr,
	JString* playerName
	)
	const
{
	*serverAddr = itsServerAddr->GetText();
	*playerName = itsPlayerName->GetText();
}

/******************************************************************************
 Receive (protected)

	Handle extra buttons.

 ******************************************************************************/

void
WizConnectDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kWizConnectHelpName);
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
