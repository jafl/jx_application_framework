/******************************************************************************
 SyGEditPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "SyGEditPrefsDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXSaveFileInput.h>
#include <JXFontManager.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGEditPrefsDialog::SyGEditPrefsDialog
	(
	const JString&	terminalCmd,
	const JString&	manViewCmd,
	const JString&	gitStatusCmd,
	const JString&	gitHistoryCmd,
	const JString&	postCheckoutCmd,
	const bool	del,
	const bool	newWindows,
	const bool	perFolderPrefs
	)
	:
	JXDialogDirector(JXGetApplication(), true)
{
	BuildWindow(terminalCmd, manViewCmd, gitStatusCmd, gitHistoryCmd,
				postCheckoutCmd, del, newWindows, perFolderPrefs);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGEditPrefsDialog::~SyGEditPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SyGEditPrefsDialog::BuildWindow
	(
	const JString&	terminalCmd,
	const JString&	manViewCmd,
	const JString&	gitStatusCmd,
	const JString&	gitHistoryCmd,
	const JString&	postCheckoutCmd,
	const bool	del,
	const bool	newWindows,
	const bool	perFolderPrefs
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 470,380, JString::empty);
	assert( window != nullptr );

	itsManInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 155,190, 300,20);
	assert( itsManInput != nullptr );

	itsTerminalInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 155,210, 300,20);
	assert( itsTerminalInput != nullptr );

	itsGitStatusInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 215,260, 240,20);
	assert( itsGitStatusInput != nullptr );

	auto* terminalLabel =
		jnew JXStaticText(JGetString("terminalLabel::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,210, 145,20);
	assert( terminalLabel != nullptr );
	terminalLabel->SetToLabel();

	auto* manPageLabel =
		jnew JXStaticText(JGetString("manPageLabel::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,190, 145,20);
	assert( manPageLabel != nullptr );
	manPageLabel->SetToLabel();

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 105,350, 60,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::SyGEditPrefsDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 305,350, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::SyGEditPrefsDialog::shortcuts::JXLayout"));

	auto* cmdHint =
		jnew JXStaticText(JGetString("cmdHint::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 155,230, 300,20);
	assert( cmdHint != nullptr );
	cmdHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	cmdHint->SetToLabel();

	itsDelCB =
		jnew JXTextCheckbox(JGetString("itsDelCB::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 220,20);
	assert( itsDelCB != nullptr );

	itsAllowSpaceCB =
		jnew JXTextCheckbox(JGetString("itsAllowSpaceCB::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,120, 220,20);
	assert( itsAllowSpaceCB != nullptr );

	auto* warnLabel =
		jnew JXStaticText(JGetString("warnLabel::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,90, 310,20);
	assert( warnLabel != nullptr );
	warnLabel->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	warnLabel->SetToLabel();

	auto* branchLabel =
		jnew JXStaticText(JGetString("branchLabel::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,300, 205,20);
	assert( branchLabel != nullptr );
	branchLabel->SetToLabel();

	auto* branchHint =
		jnew JXStaticText(JGetString("branchHint::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 215,320, 240,20);
	assert( branchHint != nullptr );
	branchHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	branchHint->SetToLabel();

	auto* statusLabel =
		jnew JXStaticText(JGetString("statusLabel::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,260, 205,20);
	assert( statusLabel != nullptr );
	statusLabel->SetToLabel();

	itsGitHistoryInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 215,280, 240,20);
	assert( itsGitHistoryInput != nullptr );

	auto* historyLabel =
		jnew JXStaticText(JGetString("historyLabel::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,280, 205,20);
	assert( historyLabel != nullptr );
	historyLabel->SetToLabel();

	itsPostCheckoutInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 215,300, 240,20);
	assert( itsPostCheckoutInput != nullptr );

	itsOpenNewWindowsCB =
		jnew JXTextCheckbox(JGetString("itsOpenNewWindowsCB::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 220,20);
	assert( itsOpenNewWindowsCB != nullptr );

	auto* openHint =
		jnew JXStaticText(JGetString("openHint::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,40, 410,20);
	assert( openHint != nullptr );
	openHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	openHint->SetToLabel();

	itsFolderPrefsCB =
		jnew JXTextCheckbox(JGetString("itsFolderPrefsCB::SyGEditPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,150, 220,20);
	assert( itsFolderPrefsCB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SyGEditPrefsDialog"));
	SetButtons(okButton, cancelButton);

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	itsManInput->GetText()->SetText(manViewCmd);
	itsManInput->SetIsRequired();
	itsManInput->SetFont(font);

	itsTerminalInput->GetText()->SetText(terminalCmd);
	itsTerminalInput->SetIsRequired();
	itsTerminalInput->SetFont(font);

	itsGitStatusInput->GetText()->SetText(gitStatusCmd);
	itsGitStatusInput->SetIsRequired();
	itsGitStatusInput->SetFont(font);

	itsGitHistoryInput->GetText()->SetText(gitHistoryCmd);
	itsGitHistoryInput->SetIsRequired();
	itsGitHistoryInput->SetFont(font);

	itsPostCheckoutInput->GetText()->SetText(postCheckoutCmd);
	itsPostCheckoutInput->SetIsRequired();
	itsPostCheckoutInput->SetFont(font);

	itsOpenNewWindowsCB->SetState(newWindows);
	itsDelCB->SetState(del);
	itsAllowSpaceCB->SetState(JXSaveFileInput::WillAllowSpace());
	itsFolderPrefsCB->SetState(perFolderPrefs);
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
SyGEditPrefsDialog::GetPrefs
	(
	JString*	terminalCmd,
	JString*	manViewCmd,
	JString*	gitStatusCmd,
	JString*	gitHistoryCmd,
	JString*	postCheckoutCmd,
	bool*	del,
	bool*	newWindows,
	bool*	perFolderPrefs
	)
	const
{
	*terminalCmd     = itsTerminalInput->GetText()->GetText();
	*manViewCmd      = itsManInput->GetText()->GetText();
	*gitStatusCmd    = itsGitStatusInput->GetText()->GetText();
	*gitHistoryCmd   = itsGitHistoryInput->GetText()->GetText();
	*postCheckoutCmd = itsPostCheckoutInput->GetText()->GetText();

	*newWindows     = itsOpenNewWindowsCB->IsChecked();
	*del            = itsDelCB->IsChecked();
	*perFolderPrefs = itsFolderPrefsCB->IsChecked();

	JXSaveFileInput::ShouldAllowSpace(itsAllowSpaceCB->IsChecked());
}
