/******************************************************************************
 JXGetStringDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#include "JXGetStringDialog.h"
#include "JXWindow.h"
#include "JXStaticText.h"
#include "JXTextButton.h"
#include "JXPasswordInput.h"
#include "jXGlobals.h"
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetStringDialog::JXGetStringDialog
	(
	const JString&	windowTitle,
	const JString&	prompt,
	const JString&	initialValue,
	const bool		password
	)
	:
	JXModalDialogDirector()
{
	BuildWindow(windowTitle, prompt, initialValue, password);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGetStringDialog::~JXGetStringDialog()
{
}

/******************************************************************************
 GetString

 ******************************************************************************/

const JString&
JXGetStringDialog::GetString()
	const
{
	return itsInputField->GetText()->GetText();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXGetStringDialog::BuildWindow
	(
	const JString&	windowTitle,
	const JString&	prompt,
	const JString&	initialValue,
	const bool		password
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 310,110, JString::empty);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXGetStringDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 190,80, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXGetStringDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXGetStringDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 60,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::JXGetStringDialog::shortcuts::JXLayout"));

	itsInputField =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 270,20);
	assert( itsInputField != nullptr );

	auto* promptDisplay =
		jnew JXStaticText(JGetString("promptDisplay::JXGetStringDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 270,20);
	assert( promptDisplay != nullptr );
	promptDisplay->SetToLabel();

// end JXLayout

	window->SetTitle(windowTitle);
	SetButtons(okButton, cancelButton);

	promptDisplay->GetText()->SetText(prompt);

	if (password)
	{
		const JRect r = itsInputField->GetFrame();
		jdelete itsInputField;
		itsInputField =
			jnew JXPasswordInput(window,
				JXWidget::kHElastic, JXWidget::kFixedTop,
				r.left, r.top, r.width(), r.height());
		assert( itsInputField != nullptr );
	}
	else if (!initialValue.IsEmpty())
	{
		itsInputField->GetText()->SetText(initialValue);
	}
	itsInputField->SetIsRequired();
}
