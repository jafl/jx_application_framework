/******************************************************************************
 JXOKToCloseDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXOKToCloseDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXImageWidget.h"
#include "JXImage.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

#include "jx_un_warning.xpm"

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

	auto* window = jnew JXWindow(this, 330,110, JString::empty);

	auto* saveButton =
		jnew JXTextButton(JGetString("saveButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,80, 60,20);
	assert( saveButton != nullptr );
	saveButton->SetShortcuts(JGetString("saveButton::JXOKToCloseDialog::shortcuts::JXLayout"));

	itsDiscardButton =
		jnew JXTextButton(JGetString("itsDiscardButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,80, 80,20);
	assert( itsDiscardButton != nullptr );
	itsDiscardButton->SetShortcuts(JGetString("itsDiscardButton::JXOKToCloseDialog::shortcuts::JXLayout"));

	auto* text =
		jnew JXStaticText(JGetString("text::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
	assert( text != nullptr );

	auto* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( icon != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,80, 60,20);
	assert( cancelButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXOKToCloseDialog"));
	SetButtons(saveButton, cancelButton);
	ListenTo(itsDiscardButton, std::function([this](const JXButton::Pushed&)
	{
		itsCloseAction = JUserNotification::kDiscardData;
		EndDialog(true);
	}));

	Init(window, text, message, icon, jx_un_warning);
}
