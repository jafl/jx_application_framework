/******************************************************************************
 SVNPrefsDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright � 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SVNPrefsDialog.h"
#include "svnGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
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

	JXWindow* window = new JXWindow(this, 400,250, "");
	assert( window != NULL );

	itsCommitEditor =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,140, 220,20);
	assert( itsCommitEditor != NULL );

	itsDiffCmd =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,160, 220,20);
	assert( itsDiffCmd != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 90,220, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::SVNPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 240,220, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SVNPrefsDialog::shortcuts::JXLayout"));

	itsIntegrationRG =
		new JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,10, 140,90);
	assert( itsIntegrationRG != NULL );

	itsJCCIntegrationRB =
		new JXTextRadioButton(SVNPrefsManager::kCodeCrusader, JGetString("itsJCCIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,10, 120,20);
	assert( itsJCCIntegrationRB != NULL );

	itsCustomIntegrationRB =
		new JXTextRadioButton(SVNPrefsManager::kCustom, JGetString("itsCustomIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,60, 120,20);
	assert( itsCustomIntegrationRB != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,45, 95,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,120, 120,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 140,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 140,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	itsCmdLineIntegrationRB =
		new JXTextRadioButton(SVNPrefsManager::kCmdLine, JGetString("itsCmdLineIntegrationRB::SVNPrefsDialog::JXLayout"), itsIntegrationRG,
					JXWidget::kHElastic, JXWidget::kFixedTop, 10,35, 120,20);
	assert( itsCmdLineIntegrationRB != NULL );

	itsReloadChangedCmd =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 160,180, 220,20);
	assert( itsReloadChangedCmd != NULL );

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::SVNPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 140,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::SVNPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsIntegrationRG->SelectItem(type);

	itsCommitEditor->SetText(commitEditor);
	itsCommitEditor->SetIsRequired();
	itsCommitEditor->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsDiffCmd->SetText(diffCmd);
	itsDiffCmd->SetIsRequired();
	itsDiffCmd->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

	itsReloadChangedCmd->SetText(reloadChangedCmd);
	itsReloadChangedCmd->SetFont(JGetMonospaceFontName(), kJDefaultMonoFontSize, JFontStyle());

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
