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

	auto* window = jnew JXWindow(this, 420,100, JGetString("WindowTitle::PrefsDialog::JXLayout"));

	auto* openFileLabel =
		jnew JXStaticText(JGetString("openFileLabel::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	openFileLabel->SetToLabel(false);

	auto* openFileHint =
		jnew JXStaticText(JGetString("openFileHint::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,40, 270,20);
	openFileHint->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,70, 70,20);
	cancelButton->SetShortcuts(JGetString("cancelButton::shortcuts::PrefsDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 259,69, 72,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::PrefsDialog::JXLayout"));

	itsOpenFileInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,20, 270,20);
	itsOpenFileInput->SetIsRequired();

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsOpenFileInput->GetText()->SetText(openCmd);
	itsOpenFileInput->SetFont(JFontManager::GetDefaultMonospaceFont());

	openFileHint->SetFontSize(JFontManager::GetDefaultFontSize()-2);
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
