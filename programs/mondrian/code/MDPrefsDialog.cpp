/******************************************************************************
 MDPrefsDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "MDPrefsDialog.h"
#include "mdGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MDPrefsDialog::MDPrefsDialog
	(
	JXDirector*		supervisor,
	const JString&	data
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(data);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MDPrefsDialog::~MDPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
MDPrefsDialog::BuildWindow
	(
	const JString& data
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,290, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,250, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::MDPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 240,250, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::MDPrefsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MDPrefsDialog"));
	SetButtons(okButton, cancelButton);
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
MDPrefsDialog::GetValues
	(
	JString* data
	)
	const
{
}
