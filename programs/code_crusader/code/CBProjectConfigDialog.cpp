/******************************************************************************
 CBProjectConfigDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1999 by John Lindal.

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
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBProjectConfigDialog::CBProjectConfigDialog
	(
	JXDirector*								supervisor,
	const CBBuildManager::MakefileMethod	method,
	const JString&							targetName,
	const JString&							depListExpr,
	const JString&							updateMakefileCmd,
	const JString&							subProjectBuildCmd
	)
	:
	JXDialogDirector(supervisor, true)
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
	const JString&							targetName,
	const JString&							depListExpr,
	const JString&							updateMakefileCmd,
	const JString&							subProjectBuildCmd
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 410,380, JString::empty);
	assert( window != nullptr );

	itsMethodRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,10, 292,106);
	assert( itsMethodRG != nullptr );

	itsTargetName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,130, 280,20);
	assert( itsTargetName != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,350, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 280,350, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBProjectConfigDialog::shortcuts::JXLayout"));

	auto* targetNameLabel =
		jnew JXStaticText(JGetString("targetNameLabel::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,130, 90,20);
	assert( targetNameLabel != nullptr );
	targetNameLabel->SetToLabel();

	auto* makemakeLabel =
		jnew JXTextRadioButton(CBBuildManager::kMakemake, JGetString("makemakeLabel::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 270,20);
	assert( makemakeLabel != nullptr );

	itsDepListExpr =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,180, 370,20);
	assert( itsDepListExpr != nullptr );

	auto* dependenciesLabel =
		jnew JXStaticText(JGetString("dependenciesLabel::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,160, 370,20);
	assert( dependenciesLabel != nullptr );
	dependenciesLabel->SetToLabel();

	auto* gmakeLabel =
		jnew JXTextRadioButton(CBBuildManager::kQMake, JGetString("gmakeLabel::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 270,20);
	assert( gmakeLabel != nullptr );

	auto* manualLabel =
		jnew JXTextRadioButton(CBBuildManager::kManual, JGetString("manualLabel::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 270,20);
	assert( manualLabel != nullptr );

	itsUpdateMakefileCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,230, 370,20);
	assert( itsUpdateMakefileCmd != nullptr );

	auto* updateMakefileLabel =
		jnew JXStaticText(JGetString("updateMakefileLabel::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,210, 370,20);
	assert( updateMakefileLabel != nullptr );
	updateMakefileLabel->SetToLabel();

	itsSubProjectBuildCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,280, 370,20);
	assert( itsSubProjectBuildCmd != nullptr );

	auto* subprojLabel =
		jnew JXStaticText(JGetString("subprojLabel::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,260, 370,20);
	assert( subprojLabel != nullptr );
	subprojLabel->SetToLabel();

	auto* configInstrText =
		jnew JXStaticText(JGetString("configInstrText::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 20,315, 370,25);
	assert( configInstrText != nullptr );
	configInstrText->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBProjectConfigDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 165,350, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBProjectConfigDialog::shortcuts::JXLayout"));

	auto* cmakeLabel =
		jnew JXTextRadioButton(CBBuildManager::kCMake, JGetString("cmakeLabel::CBProjectConfigDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 270,20);
	assert( cmakeLabel != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBProjectConfigDialog"));
	SetButtons(okButton, cancelButton);

	ListenTo(itsHelpButton);

	window->AdjustSize(0, configInstrText->GetBoundsHeight() - configInstrText->GetFrameHeight());

	itsCurrentMethod = method;
	itsMethodRG->SelectItem(method);

	itsTargetName->GetText()->SetText(targetName);
	itsTargetName->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	itsDepListExpr->GetText()->SetText(depListExpr);
	itsDepListExpr->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);

	itsUpdateMakefileCmd->GetText()->SetText(updateMakefileCmd);
	itsUpdateMakefileCmd->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
	itsUpdateMakefileCmd->SetIsRequired();

	itsSubProjectBuildCmd->GetText()->SetText(subProjectBuildCmd);
	itsSubProjectBuildCmd->GetText()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
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
	*targetName         = itsTargetName->GetText()->GetText();
	*depListExpr        = itsDepListExpr->GetText()->GetText();
	*updateMakefileCmd  = itsUpdateMakefileCmd->GetText()->GetText();
	*subProjectBuildCmd = itsSubProjectBuildCmd->GetText()->GetText();
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
	const auto method =
		(CBBuildManager::MakefileMethod) itsMethodRG->GetSelectedItem();

	JString newCmd;
	if (CBBuildManager::UpdateMakeDependCmd(itsCurrentMethod, method, &newCmd))
		{
		itsUpdateMakefileCmd->GetText()->SetText(newCmd);
		}
	itsCurrentMethod = method;

	if (method == CBBuildManager::kManual)
		{
		itsTargetName->Deactivate();
		itsTargetName->SetIsRequired(false);
		itsDepListExpr->Deactivate();
		itsUpdateMakefileCmd->Focus();
		}
	else if (method == CBBuildManager::kCMake)
		{
		itsTargetName->Activate();
		itsTargetName->SetIsRequired(true);
		itsTargetName->Focus();
		itsDepListExpr->Activate();
		}
	else if (method == CBBuildManager::kQMake)
		{
		itsTargetName->Activate();
		itsTargetName->SetIsRequired(true);
		itsTargetName->Focus();
		itsDepListExpr->Deactivate();
		}
	else
		{
		itsTargetName->Activate();
		itsTargetName->SetIsRequired(true);
		itsTargetName->Focus();
		itsDepListExpr->Activate();
		}
}
