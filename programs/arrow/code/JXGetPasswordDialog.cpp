/******************************************************************************
 JXGetPasswordDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JXGetPasswordDialog.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXPasswordInput.h>
#include <JXStaticText.h>

#include <JString.h>

#include <jXGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGetPasswordDialog::JXGetPasswordDialog
	(
	JXDirector* supervisor,
	const JCharacter* prompt
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(prompt);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGetPasswordDialog::~JXGetPasswordDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXGetPasswordDialog::BuildWindow
	(
	const JCharacter* prompt
	)
{

// begin JXLayout

    JXWindow* window = new JXWindow(this, 270,100, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXGetPasswordDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 50,70, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXGetPasswordDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXGetPasswordDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 150,70, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::JXGetPasswordDialog::shortcuts::JXLayout"));

    itsInput =
        new JXPasswordInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 15,35, 240,20);
    assert( itsInput != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXGetPasswordDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 245,20);
    assert( obj1_JXLayout != NULL );

// end JXLayout

	window->SetTitle("Enter password");
	SetButtons(okButton, cancelButton);
	obj1_JXLayout->SetText(prompt);
}

/******************************************************************************
 GetPassword

 ******************************************************************************/

const JString&
JXGetPasswordDialog::GetPassword()
{
	return itsInput->GetPassword();
}
