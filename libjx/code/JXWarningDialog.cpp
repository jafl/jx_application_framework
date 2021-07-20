/******************************************************************************
 JXWarningDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXWarningDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXImageWidget.h"
#include "JXImage.h"
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_un_warning.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWarningDialog::JXWarningDialog
	(
	JXDirector*		supervisor,
	const JString&	title,
	const JString&	message
	)
	:
	JXUNDialogBase(supervisor)
{
	BuildWindow(title, message);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWarningDialog::~JXWarningDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXWarningDialog::BuildWindow
	(
	const JString& title,
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,110, JString::empty);
	assert( window != nullptr );

	auto* noButton =
		jnew JXTextButton(JGetString("noButton::JXWarningDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 200,80, 60,20);
	assert( noButton != nullptr );
	noButton->SetShortcuts(JGetString("noButton::JXWarningDialog::shortcuts::JXLayout"));

	auto* yesButton =
		jnew JXTextButton(JGetString("yesButton::JXWarningDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,80, 60,20);
	assert( yesButton != nullptr );
	yesButton->SetShortcuts(JGetString("yesButton::JXWarningDialog::shortcuts::JXLayout"));

	auto* text =
		jnew JXStaticText(JGetString("text::JXWarningDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
	assert( text != nullptr );

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( icon != nullptr );

// end JXLayout

	window->SetTitle(title);
	SetButtons(yesButton, noButton);
	noButton->SetShortcuts(JGetString("NoShortcuts::JXWarningDialog"));		// avoid the automatic escape key shortcut

	Init(window, text, message, icon, jx_un_warning);
}
