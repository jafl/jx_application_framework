/******************************************************************************
 AboutDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "AboutDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

AboutDialog::AboutDialog()
	:
	JXModalDialogDirector()
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

	auto* window = jnew JXWindow(this, 100,100, JString::empty);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::AboutDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,35, 80,30);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::AboutDialog::shortcuts::JXLayout"));
	okButton->SetFontSize(JFontManager::GetDefaultFontSize()-2);

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::AboutDialog"));
	SetButtons(okButton, nullptr);
}
