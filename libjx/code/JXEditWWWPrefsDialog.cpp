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

	auto* window = jnew JXWindow(this, 520,290, JString::empty);

	itsShowURLCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,20, 250,20);
	assert( itsShowURLCmdInput != nullptr );

	itsShowFileContentCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,80, 250,20);
	assert( itsShowFileContentCmdInput != nullptr );

	itsShowFileLocationCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,140, 250,20);
	assert( itsShowFileLocationCmdInput != nullptr );

	itsComposeMailCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,200, 250,20);
	assert( itsComposeMailCmdInput != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,260, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 330,260, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXEditWWWPrefsDialog::shortcuts::JXLayout"));

	auto* urlVarHint =
		jnew JXStaticText(JGetString("urlVarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,40, 250,20);
	assert( urlVarHint != nullptr );
	urlVarHint->SetToLabel();

	auto* urlLabel =
		jnew JXStaticText(JGetString("urlLabel::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 230,20);
	assert( urlLabel != nullptr );
	urlLabel->SetToLabel();

	auto* mailLabel =
		jnew JXStaticText(JGetString("mailLabel::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 230,20);
	assert( mailLabel != nullptr );
	mailLabel->SetToLabel();

	auto* mailVarHint =
		jnew JXStaticText(JGetString("mailVarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,220, 250,20);
	assert( mailVarHint != nullptr );
	mailVarHint->SetToLabel();

	auto* file1VarHint =
		jnew JXStaticText(JGetString("file1VarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,100, 250,20);
	assert( file1VarHint != nullptr );
	file1VarHint->SetToLabel();

	auto* file1Label =
		jnew JXStaticText(JGetString("file1Label::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 230,20);
	assert( file1Label != nullptr );
	file1Label->SetToLabel();

	auto* file2VarHint =
		jnew JXStaticText(JGetString("file2VarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,160, 250,20);
	assert( file2VarHint != nullptr );
	file2VarHint->SetToLabel();

	auto* file2Label =
		jnew JXStaticText(JGetString("file2Label::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 230,20);
	assert( file2Label != nullptr );
	file2Label->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXEditWWWPrefsDialog"));
	SetButtons(okButton, cancelButton);

	const JFont& font = JFontManager::GetDefaultMonospaceFont();

	itsShowURLCmdInput->GetText()->SetText(showURLCmd);
	itsShowURLCmdInput->SetIsRequired();
	itsShowURLCmdInput->SetFont(font);

	itsShowFileContentCmdInput->GetText()->SetText(showFileContentCmd);
	itsShowFileContentCmdInput->SetIsRequired();
	itsShowFileContentCmdInput->SetFont(font);

	itsShowFileLocationCmdInput->GetText()->SetText(showFileLocationCmd);
	itsShowFileLocationCmdInput->SetIsRequired();
	itsShowFileLocationCmdInput->SetFont(font);

	itsComposeMailCmdInput->GetText()->SetText(composeMailCmd);
	itsComposeMailCmdInput->SetIsRequired();
	itsComposeMailCmdInput->SetFont(font);
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
