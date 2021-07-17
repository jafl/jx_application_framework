/******************************************************************************
 MDPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "MDPrefsDialog.h"
#include "mdGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MDPrefsDialog::MDPrefsDialog
	(
	JXDirector*		supervisor,
	const JString&	openCmd
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow(openCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MDPrefsDialog::~MDPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
MDPrefsDialog::BuildWindow
	(
	const JString& openCmd
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 420,100, JString::empty);
	assert( window != nullptr );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,70, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::MDPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 260,70, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::MDPrefsDialog::shortcuts::JXLayout"));

	JXStaticText* openFileLabel =
		jnew JXStaticText(JGetString("openFileLabel::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	assert( openFileLabel != nullptr );
	openFileLabel->SetToLabel();

	JXStaticText* openFileHint =
		jnew JXStaticText(JGetString("openFileHint::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,40, 270,20);
	assert( openFileHint != nullptr );
	openFileHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	openFileHint->SetToLabel();

	itsOpenFileInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,20, 270,20);
	assert( itsOpenFileInput != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MDPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsOpenFileInput->GetText()->SetText(openCmd);
	itsOpenFileInput->SetIsRequired();
	itsOpenFileInput->SetFont(JFontManager::GetDefaultMonospaceFont());
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
MDPrefsDialog::GetValues
	(
	JString* openCmd
	)
	const
{
	*openCmd = itsOpenFileInput->GetText()->GetText();
}
