/******************************************************************************
 AboutDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "AboutDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JFontManager.h>
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

	JXWindow* window = jnew JXWindow(this, 100,100, JString::empty);
	assert( window != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::AboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,35, 80,30);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::AboutDialog::shortcuts::JXLayout"));
	okButton->SetFontSize(JFontManager::GetDefaultFontSize()-2);

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::AboutDialog"));
	SetButtons(okButton, NULL);
}
