/******************************************************************************
 CBProjectConfigDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "CBProjectConfigDialog.h"
#include "cbmUtil.h"
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

	JXWindow* window = jnew JXWindow(this, 410,380, "");
	assert( window != NULL );

	itsMethodRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,10, 292,106);
	assert( itsMethodRG != NULL );

	itsTargetName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,130, 280,20);
	assert( itsTargetName != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,350, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,350, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBProjectConfigDialog::shortcuts::JXLayout"));

	JXStaticText* targetNameLabel =
		jnew JXStaticText(JGetString("targetNameLabel::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 90,20);
	assert( targetNameLabel != NULL );
	targetNameLabel->SetToLabel();

	JXTextRadioButton* makemakeLabel =
		jnew JXTextRadioButton(CBBuildManager::kMakemake, JGetString("makemakeLabel::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 270,20);
	assert( makemakeLabel != NULL );

	itsDepListExpr =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,180, 370,20);
	assert( itsDepListExpr != NULL );

	JXStaticText* dependenciesLabel =
		jnew JXStaticText(JGetString("dependenciesLabel::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,160, 370,20);
	assert( dependenciesLabel != NULL );
	dependenciesLabel->SetToLabel();

	JXTextRadioButton* gmakeLabel =
		jnew JXTextRadioButton(CBBuildManager::kQMake, JGetString("gmakeLabel::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 270,20);
	assert( gmakeLabel != NULL );

	JXTextRadioButton* manualLabel =
		jnew JXTextRadioButton(CBBuildManager::kManual, JGetString("manualLabel::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 270,20);
	assert( manualLabel != NULL );

	itsUpdateMakefileCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,230, 370,20);
	assert( itsUpdateMakefileCmd != NULL );

	JXStaticText* updateMakefileLabel =
		jnew JXStaticText(JGetString("updateMakefileLabel::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,210, 370,20);
	assert( updateMakefileLabel != NULL );
	updateMakefileLabel->SetToLabel();

	itsSubProjectBuildCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,280, 370,20);
	assert( itsSubProjectBuildCmd != NULL );

	JXStaticText* subprojLabel =
		jnew JXStaticText(JGetString("subprojLabel::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,260, 370,20);
	assert( subprojLabel != NULL );
	subprojLabel->SetToLabel();

	JXStaticText* configInstrText =
		jnew JXStaticText(JGetString("configInstrText::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 20,315, 370,25);
	assert( configInstrText != NULL );
	configInstrText->SetFontSize(JGetDefaultFontSize()-2);

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 165,350, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBProjectConfigDialog::shortcuts::JXLayout"));

	JXTextRadioButton* cmakeLabel =
		jnew JXTextRadioButton(CBBuildManager::kCMake, JGetString("cmakeLabel::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 270,20);
	assert( cmakeLabel != NULL );

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
		(JXGetHelpManager())->ShowSection("CBProjectHelp-Config");
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
