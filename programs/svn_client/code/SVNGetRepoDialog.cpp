/******************************************************************************
 SVNGetRepoDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <svnStdInc.h>
#include "SVNGetRepoDialog.h"
#include "svnGlobals.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStringHistoryMenu.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

const JFileVersion kCurrentPrefsVersion = 1;

	// version  1 adds window geometry

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNGetRepoDialog::SVNGetRepoDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	windowTitle
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	JPrefObject(SVNGetPrefsManager(), kSVNGetRepoDialogID)
{
	UseModalPlacement(kJFalse);
	BuildWindow(windowTitle);

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNGetRepoDialog::~SVNGetRepoDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 GetRepo

 ******************************************************************************/

const JString&
SVNGetRepoDialog::GetRepo()
	const
{
	const JString& s = itsRepoInput->GetText();
	itsRepoHistoryMenu->AddString(s);
	return s;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SVNGetRepoDialog::BuildWindow
	(
	const JCharacter* windowTitle
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 310,110, "");
	assert( window != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SVNGetRepoDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 190,80, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SVNGetRepoDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SVNGetRepoDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 60,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::SVNGetRepoDialog::shortcuts::JXLayout"));

	itsRepoInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 240,20);
	assert( itsRepoInput != NULL );

	itsRepoHistoryMenu =
		new JXStringHistoryMenu(kHistoryLength, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 30,20);
	assert( itsRepoHistoryMenu != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::SVNGetRepoDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 270,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle(windowTitle);
	window->LockCurrentMinSize();
	SetButtons(okButton, cancelButton);

	itsRepoInput->SetIsRequired();
	ListenTo(itsRepoHistoryMenu);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNGetRepoDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsRepoHistoryMenu && message.Is(JXMenu::kItemSelected))
		{
		itsRepoInput->SetText(itsRepoHistoryMenu->GetItemText(message));
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
SVNGetRepoDialog::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
		{
		return;
		}

	if (vers >= 1)
		{
		(GetWindow())->ReadGeometry(input);
		}
	else
		{
		(GetWindow())->PlaceAsDialogWindow();
		}

	itsRepoHistoryMenu->ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
SVNGetRepoDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;
	output << ' ';
	(GetWindow())->WriteGeometry(output);
	output << ' ';
	itsRepoHistoryMenu->WriteSetup(output);
}
