/******************************************************************************
 SVNPrefsDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 2008 by John Lindal. All rights reserved.

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
	const JCharacter*					commitEditor,
	const JCharacter*					diffCmd,
	const JCharacter*					reloadChangedCmd
	)
	:
	JXDialogDirector(supervisor, kJTrue)
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

	*commitEditor     = itsCommitEditor->GetText();
	*diffCmd          = itsDiffCmd->GetText();
	*reloadChangedCmd = itsReloadChangedCmd->GetText();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
SVNPrefsDialog::BuildWindow
	(
	const SVNPrefsManager::Integration	type,
	const JCharacter*					commitEditor,
	const JCharacter*					diffCmd,
	const JCharacter*					reloadChangedCmd
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,250, "");
	assert( window != NULL );

	itsCommitEditor =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,140, 220,20);
	assert( itsCommitEditor != NULL );

	itsDiffCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,160, 220,20);
	assert( itsDiffCmd != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 90,220, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::SVNPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 240,220, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SVNPrefsDialog::shortcuts::JXLayout"));

	itsIntegrationRG =
		jnew JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,10, 140,90);
	assert( itsIntegrationRG != NULL );

	itsJCCIntegrationRB =
		jnew JXTextRadioButton(SVNPrefsManager::kCodeCrusader, JGetString("itsJCCIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 120,20);
	assert( itsJCCIntegrationRB != NULL );

	itsCustomIntegrationRB =
		jnew JXTextRadioButton(SVNPrefsManager::kCustom, JGetString("itsCustomIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,60, 120,20);
	assert( itsCustomIntegrationRB != NULL );

	JXStaticText* integrateWithLabel =
		jnew JXStaticText(JGetString("integrateWithLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,45, 95,20);
	assert( integrateWithLabel != NULL );
	integrateWithLabel->SetToLabel();

	JXStaticText* customIntegrationLabel =
		jnew JXStaticText(JGetString("customIntegrationLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,120, 120,20);
	assert( customIntegrationLabel != NULL );
	customIntegrationLabel->SetToLabel();

	JXStaticText* reloadChangedFilesLabel =
		jnew JXStaticText(JGetString("reloadChangedFilesLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 140,20);
	assert( reloadChangedFilesLabel != NULL );
	reloadChangedFilesLabel->SetToLabel();

	JXStaticText* compareRevisionsLabel =
		jnew JXStaticText(JGetString("compareRevisionsLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 140,20);
	assert( compareRevisionsLabel != NULL );
	compareRevisionsLabel->SetToLabel();

	itsCmdLineIntegrationRB =
		jnew JXTextRadioButton(SVNPrefsManager::kCmdLine, JGetString("itsCmdLineIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,35, 120,20);
	assert( itsCmdLineIntegrationRB != NULL );

	itsReloadChangedCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,180, 220,20);
	assert( itsReloadChangedCmd != NULL );

	JXStaticText* commitEditorLabel =
		jnew JXStaticText(JGetString("commitEditorLabel::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 140,20);
	assert( commitEditorLabel != NULL );
	commitEditorLabel->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SVNPrefsDialog"));
	SetButtons(okButton, cancelButton);

	const JFontManager* fontMgr = window->GetFontManager();

	itsIntegrationRG->SelectItem(type);

	itsCommitEditor->SetText(commitEditor);
	itsCommitEditor->SetIsRequired();
	itsCommitEditor->SetFont(fontMgr->GetDefaultMonospaceFont());

	itsDiffCmd->SetText(diffCmd);
	itsDiffCmd->SetIsRequired();
	itsDiffCmd->SetFont(fontMgr->GetDefaultMonospaceFont());

	itsReloadChangedCmd->SetText(reloadChangedCmd);
	itsReloadChangedCmd->SetFont(fontMgr->GetDefaultMonospaceFont());

	UpdateDisplay();

	ListenTo(itsIntegrationRG);
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
SVNPrefsDialog::UpdateDisplay()
{
	if (JProgramAvailable("jcc"))
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

	const JBoolean enableCmds = JI2B(
		itsIntegrationRG->GetSelectedItem() == SVNPrefsManager::kCustom);

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
