/******************************************************************************
 PrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "PrefsDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PrefsDialog::PrefsDialog
	(
	const JString& openCmd
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(openCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PrefsDialog::~PrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
PrefsDialog::BuildWindow
	(
	const JString& openCmd
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 420,100, JString::empty);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,70, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::PrefsDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 260,70, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::PrefsDialog::shortcuts::JXLayout"));

	auto* openFileLabel =
		jnew JXStaticText(JGetString("openFileLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	assert( openFileLabel != nullptr );
	openFileLabel->SetToLabel();

	auto* openFileHint =
		jnew JXStaticText(JGetString("openFileHint::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,40, 270,20);
	assert( openFileHint != nullptr );
	openFileHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	openFileHint->SetToLabel();

	itsOpenFileInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,20, 270,20);
	assert( itsOpenFileInput != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::PrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsOpenFileInput->GetText()->SetText(openCmd);
	itsOpenFileInput->SetIsRequired();
	itsOpenFileInput->SetFont(JFontManager::GetDefaultMonospaceFont());
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
PrefsDialog::GetValues
	(
	JString* openCmd
	)
	const
{
	*openCmd = itsOpenFileInput->GetText()->GetText();
}
