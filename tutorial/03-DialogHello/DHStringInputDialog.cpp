/******************************************************************************
 DHStringInputDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "DHStringInputDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DHStringInputDialog::DHStringInputDialog
	(
	const JString& str
	)
	:
	JXModalDialogDirector()
{
	BuildWindow();

	// Set the initial value of the input string to the value passed in.
	itsText->GetText()->SetText(str);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
DHStringInputDialog::BuildWindow()
{
	// Create the window and pass it to the director.
	JXWindow* window = jnew JXWindow(this, 280,90, JGetString("DialogTitle::DHStringInputDialog"));

	// Create the cancel button.
	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("CancelLabel::JXGlobal"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,60, 50,20);

	// Create the OK button.
	JXTextButton* okButton =
		jnew JXTextButton(JGetString("OKLabel::JXGlobal"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 150,60, 50,20);
	okButton->SetShortcuts(JGetString("OKShortcut::JXGlobal"));

	// Create the string input field.
	itsText =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,20, 150,20);

	// Create a label for the input field.
	jnew JXStaticText(JGetString("Prompt::DHStringInputDialog"), window,
				JXWidget::kHElastic, JXWidget::kVElastic, 30,20, 65,20);

	// The dialog director needs to know what its cancel and OK buttons are.
	// The OK button must exist. The Cancel button may be nullptr.
	SetButtons(okButton, cancelButton);

	// Set it such that the input can only be 30 characters or less.
	itsText->SetMaxLength(30);
}

/******************************************************************************
 GetString

 ******************************************************************************/

JString
DHStringInputDialog::GetString()
	const
{
	// After the dialog has been deactivated, this is how the program will
	// access the new string that was typed into the input field.
	return itsText->GetText()->GetText();
}
