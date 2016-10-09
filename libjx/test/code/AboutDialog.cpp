/******************************************************************************
 AboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

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

	JXWindow* window = jnew JXWindow(this, 100,100, "");
	assert( window != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::AboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,35, 80,30);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::AboutDialog::shortcuts::JXLayout"));
    okButton->SetFontSize(8);

// end JXLayout

	window->SetTitle("About this silly program");
	SetButtons(okButton, NULL);
}
