/******************************************************************************
 ASKDialog.cc

	Asks for the passphrase and prints it to stdout.

	BASE CLASS = public JXDialogDirector

	Copyright © 2006 by New Planet Software, Inc.. All rights reserved.

 *****************************************************************************/

#include <askStdInc.h>
#include "ASKDialog.h"
#include "ASKApp.h"
#include "askGlobals.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXPasswordInput.h>
#include <JXTextButton.h>
#include <jAssert.h>

// string ID's

/******************************************************************************
 Constructor

 *****************************************************************************/

ASKDialog::ASKDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

ASKDialog::~ASKDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
ASKDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 280,100, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::ASKDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kVElastic, 30,70, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::ASKDialog::shortcuts::JXLayout"));

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::ASKDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kVElastic, 180,70, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::ASKDialog::shortcuts::JXLayout"));

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::ASKDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,10, 270,20);
    assert( obj1_JXLayout != NULL );

    itsPassphraseInput =
        new JXPasswordInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,30, 260,20);
    assert( itsPassphraseInput != NULL );

// end JXLayout

	window->SetTitle("OpenSSH");
	window->LockCurrentSize();

	SetButtons(okButton, cancelButton);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
ASKDialog::OKToDeactivate()
{
	if (JXDialogDirector::OKToDeactivate())
		{
		cout << itsPassphraseInput->GetText() << endl;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
