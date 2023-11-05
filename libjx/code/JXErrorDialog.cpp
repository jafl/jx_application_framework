/******************************************************************************
 JXErrorDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXErrorDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXImageWidget.h"
#include "JXImage.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

#include <jx-af/image/jx/jx_un_error.xpm>

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

	auto* window = jnew JXWindow(this, 330,110, JString::empty);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXErrorDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 140,80, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXErrorDialog::shortcuts::JXLayout"));

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);

	auto* text =
		jnew JXStaticText(JString::empty, true, true, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 250,50);

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXErrorDialog"));
	SetButtons(okButton, nullptr);

	text->SetBorderWidth(0);
	text->SetBackgroundColor(JColorManager::GetDefaultBackColor());

	Init(window, text, message, icon, jx_un_error);
}
