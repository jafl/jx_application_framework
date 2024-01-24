/******************************************************************************
 JXEditWWWPrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1998-2000 by John Lindal.

 ******************************************************************************/

#include "JXEditWWWPrefsDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXInputField.h"
#include "JXFontManager.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXEditWWWPrefsDialog::JXEditWWWPrefsDialog
	(
	const JString&	showURLCmd,
	const JString&	showFileContentCmd,
	const JString&	showFileLocationCmd,
	const JString&	composeMailCmd
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(showURLCmd, showFileContentCmd, showFileLocationCmd, composeMailCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXEditWWWPrefsDialog::~JXEditWWWPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXEditWWWPrefsDialog::BuildWindow
	(
	const JString& showURLCmd,
	const JString& showFileContentCmd,
	const JString& showFileLocationCmd,
	const JString& composeMailCmd
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 520,290, JGetString("WindowTitle::JXEditWWWPrefsDialog::JXLayout"));

	auto* urlLabel =
		jnew JXStaticText(JGetString("urlLabel::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 230,20);
	urlLabel->SetToLabel(false);

	auto* urlVarHint =
		jnew JXStaticText(JGetString("urlVarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,40, 250,20);
	urlVarHint->SetToLabel(false);

	auto* file1Label =
		jnew JXStaticText(JGetString("file1Label::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 230,20);
	file1Label->SetToLabel(false);

	auto* file1VarHint =
		jnew JXStaticText(JGetString("file1VarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,100, 250,20);
	file1VarHint->SetToLabel(false);

	auto* file2Label =
		jnew JXStaticText(JGetString("file2Label::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 230,20);
	file2Label->SetToLabel(false);

	auto* file2VarHint =
		jnew JXStaticText(JGetString("file2VarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,160, 250,20);
	file2VarHint->SetToLabel(false);

	auto* mailLabel =
		jnew JXStaticText(JGetString("mailLabel::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 230,20);
	mailLabel->SetToLabel(false);

	auto* mailVarHint =
		jnew JXStaticText(JGetString("mailVarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,220, 250,20);
	mailVarHint->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,260, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 329,259, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXEditWWWPrefsDialog::JXLayout"));

	itsShowURLCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,20, 250,20);
	itsShowURLCmdInput->SetIsRequired();

	itsShowFileContentCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,80, 250,20);
	itsShowFileContentCmdInput->SetIsRequired();

	itsShowFileLocationCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,140, 250,20);
	itsShowFileLocationCmdInput->SetIsRequired();

	itsComposeMailCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,200, 250,20);
	itsComposeMailCmdInput->SetIsRequired();

// end JXLayout

	SetButtons(okButton, cancelButton);

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	itsShowURLCmdInput->GetText()->SetText(showURLCmd);
	itsShowURLCmdInput->SetFont(font);
	urlVarHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	itsShowFileContentCmdInput->GetText()->SetText(showFileContentCmd);
	itsShowFileContentCmdInput->SetFont(font);
	file1VarHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	itsShowFileLocationCmdInput->GetText()->SetText(showFileLocationCmd);
	itsShowFileLocationCmdInput->SetFont(font);
	file2VarHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);

	itsComposeMailCmdInput->GetText()->SetText(composeMailCmd);
	itsComposeMailCmdInput->SetFont(font);
	mailVarHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
}

/******************************************************************************
 GetPrefs

 ******************************************************************************/

void
JXEditWWWPrefsDialog::GetPrefs
	(
	JString* showURLCmd,
	JString* showFileContentCmd,
	JString* showFileLocationCmd,
	JString* composeMailCmd
	)
	const
{
	*showURLCmd          = itsShowURLCmdInput->GetText()->GetText();
	*showFileContentCmd  = itsShowFileContentCmdInput->GetText()->GetText();
	*showFileLocationCmd = itsShowFileLocationCmdInput->GetText()->GetText();
	*composeMailCmd      = itsComposeMailCmdInput->GetText()->GetText();
}
