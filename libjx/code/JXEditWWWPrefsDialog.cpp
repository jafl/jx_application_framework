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

	JXStaticText* obj1_JXLayout =
		jnew JXStaticText(JGetString("obj1_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,40, 252,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		jnew JXStaticText(JGetString("obj2_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 230,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXStaticText* obj3_JXLayout =
		jnew JXStaticText(JGetString("obj3_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,200, 230,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	JXStaticText* obj4_JXLayout =
		jnew JXStaticText(JGetString("obj4_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,220, 250,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	JXStaticText* obj5_JXLayout =
		jnew JXStaticText(JGetString("obj5_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,100, 251,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

	JXStaticText* obj6_JXLayout =
		jnew JXStaticText(JGetString("obj6_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 230,20);
	assert( obj6_JXLayout != NULL );
	obj6_JXLayout->SetToLabel();

	JXStaticText* obj7_JXLayout =
		jnew JXStaticText(JGetString("obj7_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,160, 260,20);
	assert( obj7_JXLayout != NULL );
	obj7_JXLayout->SetToLabel();

	JXStaticText* obj8_JXLayout =
		jnew JXStaticText(JGetString("obj8_JXLayout::JXEditWWWPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,140, 230,20);
	assert( obj8_JXLayout != NULL );
	obj8_JXLayout->SetToLabel();

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
