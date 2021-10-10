/******************************************************************************
 CBStringInputDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include "CBStringInputDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBStringInputDialog::CBStringInputDialog
	(
	JXWindowDirector*	supervisor,
	const JCharacter* str
	)
	:
	JXDialogDirector(supervisor, true)
{
	BuildWindow();

	// Set the initial value of the input string to the value passed in.
	itsText->SetText(str);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBStringInputDialog::BuildWindow()
{
	// Create the window and pass it to the director.
	JXWindow* window = jnew JXWindow(this, 280,90, "Change Text");
	assert( window != nullptr );

	// Create the cancel button.
	JXTextButton* cancelButton =
		jnew JXTextButton("Cancel", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 70,60, 50,20);
	assert( cancelButton != nullptr );

	// Create the OK button.
	JXTextButton* okButton =
		jnew JXTextButton("OK", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 150,60, 50,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts("^M");

	// Create the string input field.
	itsText =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,20, 150,20);
	assert( itsText != nullptr );

	// Create a label for the input field.
	JXStaticText* obj1 =
		jnew JXStaticText("New Text:", window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,20, 65,20);
	assert( obj1 != nullptr );

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
CBStringInputDialog::GetString()
	const
{
	// After the dialog has been deactivated, this is how the program will
	// access the new string that was typed into the input field.
	return itsText->GetText();
}
