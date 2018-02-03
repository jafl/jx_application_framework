/******************************************************************************
 <PRE>PrefsDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) <Year> by <Company>.

 ******************************************************************************/

#include "<PRE>PrefsDialog.h"
#include "<pre>Globals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

<PRE>PrefsDialog::<PRE>PrefsDialog
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

<PRE>PrefsDialog::~<PRE>PrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
<PRE>PrefsDialog::BuildWindow
	(
	const JString& data
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,290, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::<PRE>PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,250, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::<PRE>PrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::<PRE>PrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 240,250, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::<PRE>PrefsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::<PRE>PrefsDialog"));
	SetButtons(okButton, cancelButton);
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
<PRE>PrefsDialog::GetValues
	(
	JString* data
	)
	const
{
}
