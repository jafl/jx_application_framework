/******************************************************************************
 JXOKToCloseDialog.cpp

	Since this dialog has three possible responses, clients should listen
	for GotResponse() instead of Deactivated().

	BASE CLASS = JXUNDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXOKToCloseDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXStaticText.h"
#include "JXImageWidget.h"
#include "JXImage.h"
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_un_warning.xpm>

// JBroadcaster message types

const JUtf8Byte* JXOKToCloseDialog::kGotResponse = "GotResponse::JXOKToCloseDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXOKToCloseDialog::JXOKToCloseDialog
	(
	JXDirector*		supervisor,
	const JString&	message
	)
	:
	JXUNDialogBase(supervisor)
{
	BuildWindow(message);
	ListenTo(this);
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

	JXWindow* window = jnew JXWindow(this, 330,110, JString::empty);
	assert( window != nullptr );

	JXTextButton* saveButton =
		jnew JXTextButton(JGetString("saveButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,80, 60,20);
	assert( saveButton != nullptr );
	saveButton->SetShortcuts(JGetString("saveButton::JXOKToCloseDialog::shortcuts::JXLayout"));

	itsDiscardButton =
		jnew JXTextButton(JGetString("itsDiscardButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,80, 80,20);
	assert( itsDiscardButton != nullptr );
	itsDiscardButton->SetShortcuts(JGetString("itsDiscardButton::JXOKToCloseDialog::shortcuts::JXLayout"));

	JXStaticText* text =
		jnew JXStaticText(JGetString("text::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
	assert( text != nullptr );

	JXImageWidget* icon =
		jnew JXImageWidget(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
	assert( icon != nullptr );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXOKToCloseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,80, 60,20);
	assert( cancelButton != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXOKToCloseDialog"));
	SetButtons(saveButton, cancelButton);
	ListenTo(itsDiscardButton);

	Init(window, text, message, icon, jx_un_warning);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXOKToCloseDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == this && message.Is(kDeactivated))
		{
		const Deactivated* info = dynamic_cast<const Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			Broadcast(GotResponse(JUserNotification::kSaveData));
			}
		else
			{
			Broadcast(GotResponse(JUserNotification::kDontClose));
			}
		}

	else if (sender == itsDiscardButton && message.Is(JXButton::kPushed))
		{
		Broadcast(GotResponse(JUserNotification::kDiscardData));
		const bool ok = Close();
		assert( ok );
		}

	else
		{
		JXUNDialogBase::Receive(sender, message);
		}
}
