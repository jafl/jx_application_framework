/******************************************************************************
 AboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "AboutDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

AboutDialog::AboutDialog
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

 ******************************************************************************/

AboutDialog::~AboutDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
AboutDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 100,100, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::AboutDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,35, 80,30);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::AboutDialog::shortcuts::JXLayout"));
    okButton->SetFontSize(10);

// end JXLayout

	window->SetTitle("About this silly program");
	SetButtons(okButton, NULL);
}
