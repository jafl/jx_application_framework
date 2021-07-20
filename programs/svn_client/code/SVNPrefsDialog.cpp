/******************************************************************************
 SVNPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "SVNPrefsDialog.h"
#include "svnGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JFontManager.h>
#include <jProcessUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNPrefsDialog::SVNPrefsDialog
	(
	JXDirector*							supervisor,
	const SVNPrefsManager::Integration	type,
	const JString&						commitEditor,
	const JString&						diffCmd,
	const JString&						reloadChangedCmd
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow(type, commitEditor,diffCmd, reloadChangedCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNPrefsDialog::~SVNPrefsDialog()
{
}

/******************************************************************************
 GetData

 ******************************************************************************/

void
SVNPrefsDialog::GetData
	(
	SVNPrefsManager::Integration*	type,
	JString*						commitEditor,
	JString*						diffCmd,
	JString*						reloadChangedCmd
	)
	const
{
	*type = (SVNPrefsManager::Integration) itsIntegrationRG->GetSelectedItem();

	*commitEditor     = itsCommitEditor->GetText()->GetText();
	*diffCmd          = itsDiffCmd->GetText()->GetText();
	*reloadChangedCmd = itsReloadChangedCmd->GetText()->GetText();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SVNPrefsDialog::BuildWindow
	(
	const SVNPrefsManager::Integration	type,
	const JString&						commitEditor,
	const JString&						diffCmd,
	const JString&						reloadChangedCmd
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,250, JString::empty);
	assert( window != nullptr );

	itsCommitEditor =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,140, 220,20);
	assert( itsCommitEditor != nullptr );

	itsDiffCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,160, 220,20);
	assert( itsDiffCmd != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 90,220, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::SVNPrefsDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 240,220, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::SVNPrefsDialog::shortcuts::JXLayout"));

	itsIntegrationRG =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,10, 140,90);
	assert( itsIntegrationRG != nullptr );

	itsJCCIntegrationRB =
		jnew JXTextRadioButton(SVNPrefsManager::kCodeCrusader, JGetString("itsJCCIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 120,20);
	assert( itsJCCIntegrationRB != nullptr );

	itsCustomIntegrationRB =
		jnew JXTextRadioButton(SVNPrefsManager::kCustom, JGetString("itsCustomIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,60, 120,20);
	assert( itsCustomIntegrationRB != nullptr );

	auto* integrateWithLabel =
		jnew JXStaticText(JGetString("integrateWithLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,45, 95,20);
	assert( integrateWithLabel != nullptr );
	integrateWithLabel->SetToLabel();

	auto* customIntegrationLabel =
		jnew JXStaticText(JGetString("customIntegrationLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,120, 120,20);
	assert( customIntegrationLabel != nullptr );
	customIntegrationLabel->SetToLabel();

	auto* reloadChangedFilesLabel =
		jnew JXStaticText(JGetString("reloadChangedFilesLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 140,20);
	assert( reloadChangedFilesLabel != nullptr );
	reloadChangedFilesLabel->SetToLabel();

	auto* compareRevisionsLabel =
		jnew JXStaticText(JGetString("compareRevisionsLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 140,20);
	assert( compareRevisionsLabel != nullptr );
	compareRevisionsLabel->SetToLabel();

	itsCmdLineIntegrationRB =
		jnew JXTextRadioButton(SVNPrefsManager::kCmdLine, JGetString("itsCmdLineIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,35, 120,20);
	assert( itsCmdLineIntegrationRB != nullptr );

	itsReloadChangedCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,180, 220,20);
	assert( itsReloadChangedCmd != nullptr );

	auto* commitEditorLabel =
		jnew JXStaticText(JGetString("commitEditorLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 140,20);
	assert( commitEditorLabel != nullptr );
	commitEditorLabel->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SVNPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsIntegrationRG->SelectItem(type);

	itsCommitEditor->GetText()->SetText(commitEditor);
	itsCommitEditor->SetIsRequired();
	itsCommitEditor->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsDiffCmd->GetText()->SetText(diffCmd);
	itsDiffCmd->SetIsRequired();
	itsDiffCmd->SetFont(JFontManager::GetDefaultMonospaceFont());

	itsReloadChangedCmd->GetText()->SetText(reloadChangedCmd);
	itsReloadChangedCmd->SetFont(JFontManager::GetDefaultMonospaceFont());

	UpdateDisplay();

	ListenTo(itsIntegrationRG);
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
SVNPrefsDialog::UpdateDisplay()
{
	if (JProgramAvailable(JGetString("CodeCrusaderBinary::SVNGlobal")))
		{
		itsJCCIntegrationRB->Activate();
		}
	else
		{
		itsJCCIntegrationRB->Deactivate();
		if (itsJCCIntegrationRB->IsChecked())
			{
			itsCmdLineIntegrationRB->Select();
			}
		}

	const bool enableCmds = itsIntegrationRG->GetSelectedItem() == SVNPrefsManager::kCustom;

	itsCommitEditor->SetActive(enableCmds);
	itsDiffCmd->SetActive(enableCmds);
	itsReloadChangedCmd->SetActive(enableCmds);

	if (enableCmds)
		{
		itsCommitEditor->Focus();
		}
	else
		{
		GetWindow()->KillFocus();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNPrefsDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsIntegrationRG && message.Is(JXRadioGroup::kSelectionChanged))
		{
		UpdateDisplay();
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
