/******************************************************************************
 DHStringInputDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include <JXStdInc.h>
#include "DHStringInputDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

DHStringInputDialog::DHStringInputDialog
	(
	JXWindowDirector*	supervisor,
	const JCharacter* str
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
	
	// Set the initial value of the input string to the value passed in.
	itsText->SetText(str);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

DHStringInputDialog::~DHStringInputDialog()
{
	// We don't have anything to delete.
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
DHStringInputDialog::BuildWindow()
{
	// Create the window and pass it to the director.
    JXWindow* window = new JXWindow(this, 280,90, "Change Text");
    assert( window != NULL );
    SetWindow(window);

	// Create the cancel button. 
    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 70,60, 50,20);
    assert( cancelButton != NULL );

	// Create the OK button.
    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 150,60, 50,20);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

	// Create the string input field.
    itsText =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 100,20, 150,20);
    assert( itsText != NULL );

	// Create a label for the input field.
    JXStaticText* obj1 =
        new JXStaticText("New Text:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 30,20, 65,20);
    assert( obj1 != NULL );

	// The dialog director needs to know what its cancel and OK buttons are.
	// The OK button must exist. The Cancel button may be NULL.
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
	return itsText->GetText();
}
