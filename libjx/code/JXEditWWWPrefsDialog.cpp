/******************************************************************************
 JXEditWWWPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1998-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXEditWWWPrefsDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <JXFontManager.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXEditWWWPrefsDialog::JXEditWWWPrefsDialog
	(
	JXDirector*		supervisor,
	const JString&	showURLCmd,
	const JString&	showFileContentCmd,
	const JString&	showFileLocationCmd,
	const JString&	composeMailCmd
	)
	:
	JXDialogDirector(supervisor, kJTrue)
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

	JXWindow* window = jnew JXWindow(this, 520,290, JString::empty);
	assert( window != NULL );

	itsShowURLCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,20, 250,20);
	assert( itsShowURLCmdInput != NULL );

	itsShowFileContentCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,80, 250,20);
	assert( itsShowFileContentCmdInput != NULL );

	itsShowFileLocationCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,140, 250,20);
	assert( itsShowFileLocationCmdInput != NULL );

	itsComposeMailCmdInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 250,200, 250,20);
	assert( itsComposeMailCmdInput != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 120,260, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 330,260, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXEditWWWPrefsDialog::shortcuts::JXLayout"));

	JXStaticText* urlVarHint =
		jnew JXStaticText(JGetString("urlVarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,40, 250,20);
	assert( urlVarHint != NULL );
	urlVarHint->SetToLabel();

	JXStaticText* urlLabel =
		jnew JXStaticText(JGetString("urlLabel::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 230,20);
	assert( urlLabel != NULL );
	urlLabel->SetToLabel();

	JXStaticText* mailLabel =
		jnew JXStaticText(JGetString("mailLabel::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 230,20);
	assert( mailLabel != NULL );
	mailLabel->SetToLabel();

	JXStaticText* mailVarHint =
		jnew JXStaticText(JGetString("mailVarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,220, 250,20);
	assert( mailVarHint != NULL );
	mailVarHint->SetToLabel();

	JXStaticText* file1VarHint =
		jnew JXStaticText(JGetString("file1VarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,100, 250,20);
	assert( file1VarHint != NULL );
	file1VarHint->SetToLabel();

	JXStaticText* file1Label =
		jnew JXStaticText(JGetString("file1Label::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 230,20);
	assert( file1Label != NULL );
	file1Label->SetToLabel();

	JXStaticText* file2VarHint =
		jnew JXStaticText(JGetString("file2VarHint::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,160, 250,20);
	assert( file2VarHint != NULL );
	file2VarHint->SetToLabel();

	JXStaticText* file2Label =
		jnew JXStaticText(JGetString("file2Label::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 230,20);
	assert( file2Label != NULL );
	file2Label->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXEditWWWPrefsDialog"));
	SetButtons(okButton, cancelButton);

	const JFont& font = (window->GetFontManager())->GetDefaultMonospaceFont();

	itsShowURLCmdInput->SetText(showURLCmd);
	itsShowURLCmdInput->SetIsRequired();
	itsShowURLCmdInput->SetFont(font);

	itsShowFileContentCmdInput->SetText(showFileContentCmd);
	itsShowFileContentCmdInput->SetIsRequired();
	itsShowFileContentCmdInput->SetFont(font);

	itsShowFileLocationCmdInput->SetText(showFileLocationCmd);
	itsShowFileLocationCmdInput->SetIsRequired();
	itsShowFileLocationCmdInput->SetFont(font);

	itsComposeMailCmdInput->SetText(composeMailCmd);
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
	*showURLCmd          = itsShowURLCmdInput->GetText();
	*showFileContentCmd  = itsShowFileContentCmdInput->GetText();
	*showFileLocationCmd = itsShowFileLocationCmdInput->GetText();
	*composeMailCmd      = itsComposeMailCmdInput->GetText();
}
