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

	auto* window = jnew JXWindow(this, 420,80, JGetString("WindowTitle::PrefsDialog::JXLayout"));

	auto* openFileLabel =
		jnew JXStaticText(JGetString("openFileLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	openFileLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,50, 70,20);
	cancelButton->SetShortcuts(JGetString("cancelButton::shortcuts::PrefsDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 260,50, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::PrefsDialog::JXLayout"));

	itsOpenFileInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,20, 270,20);
	itsOpenFileInput->SetIsRequired();
	itsOpenFileInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsOpenFileInput->SetHint(JGetString("itsOpenFileInput::PrefsDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsOpenFileInput->GetText()->SetText(openCmd);
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
