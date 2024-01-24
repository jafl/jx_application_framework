/******************************************************************************
 JXErrorDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXErrorDialog.h"
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

JXErrorDialog::JXErrorDialog
	(
	const JString& message
	)
	:
	JXUNDialogBase()
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

	auto* window = jnew JXWindow(this, 330,110, JGetString("WindowTitle::JXErrorDialog::JXLayout"));

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
#ifndef _H_jx_af_image_jx_jx_un_error
#define _H_jx_af_image_jx_jx_un_error
#include <jx-af/image/jx/jx_un_error.xpm>
#endif
	icon->SetImage(GetDisplay()->GetImageCache()->GetImage(jx_un_error), false);

	auto* text =
		jnew JXStaticText(JString::empty, true, true, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 250,50);
	assert( text != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXErrorDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 139,79, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXErrorDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, nullptr);
	Init(window, text, message);
}
