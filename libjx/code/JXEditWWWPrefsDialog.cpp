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

	auto* window = jnew JXWindow(this, 520,180, JGetString("WindowTitle::JXEditWWWPrefsDialog::JXLayout"));

	auto* urlLabel =
		jnew JXStaticText(JGetString("urlLabel::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 230,20);
	urlLabel->SetToLabel(false);

	auto* file1Label =
		jnew JXStaticText(JGetString("file1Label::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 230,20);
	file1Label->SetToLabel(false);

	auto* file2Label =
		jnew JXStaticText(JGetString("file2Label::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 230,20);
	file2Label->SetToLabel(false);

	auto* mailLabel =
		jnew JXStaticText(JGetString("mailLabel::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,110, 230,20);
	mailLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,150, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 330,150, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXEditWWWPrefsDialog::JXLayout"));

	itsShowURLCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,20, 250,20);
	itsShowURLCmdInput->SetIsRequired();
	itsShowURLCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsShowURLCmdInput->SetHint(JGetString("itsShowURLCmdInput::JXEditWWWPrefsDialog::JXLayout"));

	itsShowFileContentCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,50, 250,20);
	itsShowFileContentCmdInput->SetIsRequired();
	itsShowFileContentCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsShowFileContentCmdInput->SetHint(JGetString("itsShowFileContentCmdInput::JXEditWWWPrefsDialog::JXLayout"));

	itsShowFileLocationCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,80, 250,20);
	itsShowFileLocationCmdInput->SetIsRequired();
	itsShowFileLocationCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsShowFileLocationCmdInput->SetHint(JGetString("itsShowFileLocationCmdInput::JXEditWWWPrefsDialog::JXLayout"));

	itsComposeMailCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,110, 250,20);
	itsComposeMailCmdInput->SetIsRequired();
	itsComposeMailCmdInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsComposeMailCmdInput->SetHint(JGetString("itsComposeMailCmdInput::JXEditWWWPrefsDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsShowURLCmdInput->GetText()->SetText(showURLCmd);
	itsShowFileContentCmdInput->GetText()->SetText(showFileContentCmd);
	itsShowFileLocationCmdInput->GetText()->SetText(showFileLocationCmd);
	itsComposeMailCmdInput->GetText()->SetText(composeMailCmd);
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
