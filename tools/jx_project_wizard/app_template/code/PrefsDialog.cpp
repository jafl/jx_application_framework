/******************************************************************************
 PrefsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) <Year> by <Company>.

 ******************************************************************************/

#include "PrefsDialog.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

PrefsDialog::PrefsDialog
	(
	const JString& data
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(data);
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
	const JString& data
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,290, JString::empty);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,250, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::PrefsDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 240,250, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::PrefsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::PrefsDialog"));
	SetButtons(okButton, cancelButton);
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
PrefsDialog::GetValues
	(
	JString* data
	)
	const
{
}
