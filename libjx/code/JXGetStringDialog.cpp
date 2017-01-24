/******************************************************************************
 JXGetStringDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXGetStringDialog.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXPasswordInput.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetStringDialog::JXGetStringDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	windowTitle,
	const JCharacter*	prompt,
	const JCharacter*	initialValue,
	const JBoolean		modal,
	const JBoolean		password
	)
	:
	JXDialogDirector(supervisor, modal)
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
	return itsInputField->GetText();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXGetStringDialog::BuildWindow
	(
	const JCharacter*	windowTitle,
	const JCharacter*	prompt,
	const JCharacter*	initialValue,
	const JBoolean		password
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 310,110, JString::empty);
	assert( window != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXGetStringDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 190,80, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXGetStringDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXGetStringDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 60,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::JXGetStringDialog::shortcuts::JXLayout"));

	itsInputField =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,40, 270,20);
	assert( itsInputField != NULL );

	JXStaticText* promptDisplay =
		jnew JXStaticText(JGetString("promptDisplay::JXGetStringDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 20,20, 270,20);
	assert( promptDisplay != NULL );
	promptDisplay->SetToLabel();

// end JXLayout

	window->SetTitle(windowTitle);
	SetButtons(okButton, cancelButton);

	promptDisplay->SetText(prompt);

	if (password)
		{
		const JRect r = itsInputField->GetFrame();
		jdelete itsInputField;
		itsInputField =
			jnew JXPasswordInput(window,
				JXWidget::kHElastic, JXWidget::kFixedTop,
				r.left, r.top, r.width(), r.height());
		assert( itsInputField != NULL );
		}
	else if (!JString::IsEmpty(initialValue))
		{
		itsInputField->SetText(initialValue);
		}
	itsInputField->SetIsRequired();
}
