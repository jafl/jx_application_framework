/******************************************************************************
 JXErrorDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXErrorDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_un_error.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXErrorDialog::JXErrorDialog
	(
	JXDirector*		supervisor,
	const JString&	message
	)
	:
	JXUNDialogBase(supervisor)
{
	BuildWindow(message);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXErrorDialog::~JXErrorDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXErrorDialog::BuildWindow
	(
	const JString& message
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 330,110, JString::empty);
	assert( window != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXErrorDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 140,80, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXErrorDialog::shortcuts::JXLayout"));

	JXStaticText* text =
		jnew JXStaticText(JGetString("text::JXErrorDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
	assert( text != NULL );

	JXImageWidget* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( icon != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXErrorDialog"));
	SetButtons(okButton, NULL);

	Init(window, text, message, icon, jx_un_error);
}
