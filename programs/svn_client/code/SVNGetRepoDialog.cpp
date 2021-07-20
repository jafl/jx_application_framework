/******************************************************************************
 SVNGetRepoDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

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
	JXDirector*		supervisor,
	const JString&	windowTitle
	)
	:
	JXDialogDirector(supervisor, true),
	JPrefObject(SVNGetPrefsManager(), kSVNGetRepoDialogID)
{
	UseModalPlacement(false);
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
	const JString& s = itsRepoInput->GetText()->GetText();
	itsRepoHistoryMenu->AddString(s);
	return s;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SVNGetRepoDialog::BuildWindow
	(
	const JString& windowTitle
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,110, JString::empty);
	assert( window != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::SVNGetRepoDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 190,80, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::SVNGetRepoDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SVNGetRepoDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 60,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::SVNGetRepoDialog::shortcuts::JXLayout"));

	itsRepoInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 240,20);
	assert( itsRepoInput != nullptr );

	itsRepoHistoryMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 260,40, 30,20);
	assert( itsRepoHistoryMenu != nullptr );

	auto* repoUrlLabel =
		jnew JXStaticText(JGetString("repoUrlLabel::SVNGetRepoDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 270,20);
	assert( repoUrlLabel != nullptr );
	repoUrlLabel->SetToLabel();

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
		itsRepoInput->GetText()->SetText(itsRepoHistoryMenu->GetItemText(message));
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
	std::istream& input
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
		GetWindow()->ReadGeometry(input);
		}
	else
		{
		GetWindow()->PlaceAsDialogWindow();
		}

	itsRepoHistoryMenu->ReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
SVNGetRepoDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion;
	output << ' ';
	GetWindow()->WriteGeometry(output);
	output << ' ';
	itsRepoHistoryMenu->WriteSetup(output);
}
