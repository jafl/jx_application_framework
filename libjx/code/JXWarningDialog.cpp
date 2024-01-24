/******************************************************************************
 JXWarningDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXWarningDialog.h"
#include "JXDisplay.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXImageWidget.h"
#include "JXImageCache.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWarningDialog::JXWarningDialog
	(
	const JString& title,
	const JString& message
	)
	:
	JXUNDialogBase()
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

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
#ifndef _H_jx_un_warning
#define _H_jx_un_warning
#include "jx_un_warning.xpm"
#endif
	icon->SetImage(GetDisplay()->GetImageCache()->GetImage(jx_un_warning), false);

	auto* text =
		jnew JXStaticText(JString::empty, true, true, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 250,50);
	assert( text != nullptr );

	auto* yesButton =
		jnew JXTextButton(JGetString("yesButton::JXWarningDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,80, 60,20);
	yesButton->SetShortcuts(JGetString("yesButton::shortcuts::JXWarningDialog::JXLayout"));

	auto* noButton =
		jnew JXTextButton(JGetString("noButton::JXWarningDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 200,80, 60,20);
	noButton->SetShortcuts(JGetString("noButton::shortcuts::JXWarningDialog::JXLayout"));

// end JXLayout

	window->SetTitle(title);
	SetButtons(yesButton, noButton);
	noButton->SetShortcuts(JGetString("NoShortcuts::JXWarningDialog"));		// avoid the automatic escape key shortcut

	Init(window, text, message);
}
