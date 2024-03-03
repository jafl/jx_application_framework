/******************************************************************************
 JXOKToCloseDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXOKToCloseDialog.h"
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

JXOKToCloseDialog::JXOKToCloseDialog
	(
	const JString& message
	)
	:
	JXUNDialogBase(),
	itsCloseAction(JUserNotification::kSaveData)
{
	BuildWindow(message);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXOKToCloseDialog::~JXOKToCloseDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXOKToCloseDialog::BuildWindow
	(
	const JString& message
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 330,110, JGetString("WindowTitle::JXOKToCloseDialog::JXLayout"));

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
#ifndef _H_jx_un_warning
#define _H_jx_un_warning
#include "jx_un_warning.xpm"
#endif
	icon->SetImage(GetDisplay()->GetImageCache()->GetImage(jx_un_warning), false);

	auto* text =
		jnew JXStaticText(JString::empty, true, false, false, nullptr, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
	text->SetBorderWidth(0);

	itsDiscardButton =
		jnew JXTextButton(JGetString("itsDiscardButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,80, 80,20);
	itsDiscardButton->SetShortcuts(JGetString("itsDiscardButton::shortcuts::JXOKToCloseDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,80, 60,20);
	assert( cancelButton != nullptr );

	auto* saveButton =
		jnew JXTextButton(JGetString("saveButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,80, 60,20);
	saveButton->SetShortcuts(JGetString("saveButton::shortcuts::JXOKToCloseDialog::JXLayout"));

// end JXLayout

	SetButtons(saveButton, cancelButton);
	ListenTo(itsDiscardButton, std::function([this](const JXButton::Pushed&)
	{
		itsCloseAction = JUserNotification::kDiscardData;
		EndDialog(true);
	}));

	Init(window, text, message);
}
