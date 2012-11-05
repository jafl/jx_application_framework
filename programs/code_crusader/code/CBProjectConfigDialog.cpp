/******************************************************************************
 CBProjectConfigDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <cbStdInc.h>
#include "CBProjectConfigDialog.h"
#include "cbmUtil.h"
#include "cbHelpText.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXInputField.h>
#include <JXChooseSaveFile.h>
#include <JXHelpManager.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBProjectConfigDialog::CBProjectConfigDialog
	(
	JXDirector*								supervisor,
	const CBBuildManager::MakefileMethod	method,
	const JCharacter*						targetName,
	const JCharacter*						depListExpr,
	const JCharacter*						updateMakefileCmd,
	const JCharacter*						subProjectBuildCmd
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(method, targetName, depListExpr, updateMakefileCmd, subProjectBuildCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBProjectConfigDialog::~CBProjectConfigDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBProjectConfigDialog::BuildWindow
	(
	const CBBuildManager::MakefileMethod	method,
	const JCharacter*						targetName,
	const JCharacter*						depListExpr,
	const JCharacter*						updateMakefileCmd,
	const JCharacter*						subProjectBuildCmd
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 410,380, "");
	assert( window != NULL );

	itsMethodRG =
		new JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,10, 292,106);
	assert( itsMethodRG != NULL );

	itsTargetName =
		new JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,130, 280,20);
	assert( itsTargetName != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,350, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,350, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBProjectConfigDialog::shortcuts::JXLayout"));

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 90,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXTextRadioButton* obj2_JXLayout =
		new JXTextRadioButton(CBBuildManager::kMakemake, JGetString("obj2_JXLayout::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 270,20);
	assert( obj2_JXLayout != NULL );

	itsDepListExpr =
		new JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,180, 370,20);
	assert( itsDepListExpr != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,160, 370,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	JXTextRadioButton* obj4_JXLayout =
		new JXTextRadioButton(CBBuildManager::kQMake, JGetString("obj4_JXLayout::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 270,20);
	assert( obj4_JXLayout != NULL );

	JXTextRadioButton* obj5_JXLayout =
		new JXTextRadioButton(CBBuildManager::kManual, JGetString("obj5_JXLayout::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 270,20);
	assert( obj5_JXLayout != NULL );

	itsUpdateMakefileCmd =
		new JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,230, 370,20);
	assert( itsUpdateMakefileCmd != NULL );

	JXStaticText* obj6_JXLayout =
		new JXStaticText(JGetString("obj6_JXLayout::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,210, 370,20);
	assert( obj6_JXLayout != NULL );
	obj6_JXLayout->SetToLabel();

	itsSubProjectBuildCmd =
		new JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,280, 370,20);
	assert( itsSubProjectBuildCmd != NULL );

	JXStaticText* obj7_JXLayout =
		new JXStaticText(JGetString("obj7_JXLayout::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,260, 370,20);
	assert( obj7_JXLayout != NULL );
	obj7_JXLayout->SetToLabel();

	JXStaticText* configInstrText =
		new JXStaticText(JGetString("configInstrText::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 20,315, 370,25);
	assert( configInstrText != NULL );
    configInstrText->SetFontSize(8);

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 165,350, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBProjectConfigDialog::shortcuts::JXLayout"));

	JXTextRadioButton* obj8_JXLayout =
		new JXTextRadioButton(CBBuildManager::kCMake, JGetString("obj8_JXLayout::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 270,20);
	assert( obj8_JXLayout != NULL );

// end JXLayout

	window->SetTitle("Project Configuration");
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	window->AdjustSize(0, configInstrText->GetBoundsHeight() - configInstrText->GetFrameHeight());

	itsCurrentMethod = method;
	itsMethodRG->SelectItem(method);

	itsTargetName->SetText(targetName);
	itsTargetName->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	itsDepListExpr->SetText(depListExpr);
	itsDepListExpr->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	itsUpdateMakefileCmd->SetText(updateMakefileCmd);
	itsUpdateMakefileCmd->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	itsUpdateMakefileCmd->SetIsRequired();

	itsSubProjectBuildCmd->SetText(subProjectBuildCmd);
	itsSubProjectBuildCmd->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	itsSubProjectBuildCmd->SetIsRequired();

	UpdateDisplay();
	ListenTo(itsMethodRG);
}

/******************************************************************************
 GetConfig

 ******************************************************************************/

void
CBProjectConfigDialog::GetConfig
	(
	CBBuildManager::MakefileMethod*	method,
	JString*						targetName,
	JString*						depListExpr,
	JString*						updateMakefileCmd,
	JString*						subProjectBuildCmd
	)
	const
{
	*method             = (CBBuildManager::MakefileMethod) itsMethodRG->GetSelectedItem();
	*targetName         = itsTargetName->GetText();
	*depListExpr        = itsDepListExpr->GetText();
	*updateMakefileCmd  = itsUpdateMakefileCmd->GetText();
	*subProjectBuildCmd = itsSubProjectBuildCmd->GetText();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBProjectConfigDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsMethodRG && message.Is(JXRadioGroup::kSelectionChanged))
		{
		UpdateDisplay();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kCBProjectConfigHelpName);
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateDisplay (private)

 ******************************************************************************/

void
CBProjectConfigDialog::UpdateDisplay()
{
	const CBBuildManager::MakefileMethod method =
		(CBBuildManager::MakefileMethod) itsMethodRG->GetSelectedItem();

	JString newCmd;
	if (CBBuildManager::UpdateMakeDependCmd(itsCurrentMethod, method, &newCmd))
		{
		itsUpdateMakefileCmd->SetText(newCmd);
		}
	itsCurrentMethod = method;

	if (method == CBBuildManager::kManual)
		{
		itsTargetName->Deactivate();
		itsTargetName->SetIsRequired(kJFalse);
		itsDepListExpr->Deactivate();
		itsUpdateMakefileCmd->Focus();
		}
	else if (method == CBBuildManager::kCMake)
		{
		itsTargetName->Activate();
		itsTargetName->SetIsRequired(kJTrue);
		itsTargetName->Focus();
		itsDepListExpr->Activate();
		}
	else if (method == CBBuildManager::kQMake)
		{
		itsTargetName->Activate();
		itsTargetName->SetIsRequired(kJTrue);
		itsTargetName->Focus();
		itsDepListExpr->Deactivate();
		}
	else
		{
		itsTargetName->Activate();
		itsTargetName->SetIsRequired(kJTrue);
		itsTargetName->Focus();
		itsDepListExpr->Activate();
		}
}
