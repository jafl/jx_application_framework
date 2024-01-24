/******************************************************************************
 JXMessageDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXMessageDialog.h"
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

JXMessageDialog::JXMessageDialog
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

JXMessageDialog::~JXMessageDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXMessageDialog::BuildWindow
	(
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,110, JGetString("WindowTitle::JXMessageDialog::JXLayout"));

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
#ifndef _H_jx_af_image_jx_jx_un_message
#define _H_jx_af_image_jx_jx_un_message
#include <jx-af/image/jx/jx_un_message.xpm>
#endif
	icon->SetImage(GetDisplay()->GetImageCache()->GetImage(jx_un_message), false);

	auto* text =
		jnew JXStaticText(JString::empty, true, true, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 250,50);
	assert( text != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXMessageDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 139,79, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXMessageDialog::JXLayout"));

// end JXLayout

	SetButtons(okButton, nullptr);
	Init(window, text, message);
}
