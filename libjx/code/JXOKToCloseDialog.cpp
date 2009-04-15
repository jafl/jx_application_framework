/******************************************************************************
 JXOKToCloseDialog.cpp

	Since this dialog has three possible responses, clients should listen
	for GotResponse() instead of Deactivated().

	BASE CLASS = JXUNDialogBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXOKToCloseDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_un_warning.xpm>

// JBroadcaster message types

const JCharacter* JXOKToCloseDialog::kGotResponse = "GotResponse::JXOKToCloseDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXOKToCloseDialog::JXOKToCloseDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	message
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
	const JCharacter* message
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 330,110, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* saveButton =
        new JXTextButton(JGetString("saveButton::JXOKToCloseDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 249,79, 62,22);
    assert( saveButton != NULL );
    saveButton->SetShortcuts(JGetString("saveButton::JXOKToCloseDialog::shortcuts::JXLayout"));

    itsDiscardButton =
        new JXTextButton(JGetString("itsDiscardButton::JXOKToCloseDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 50,80, 80,20);
    assert( itsDiscardButton != NULL );
    itsDiscardButton->SetShortcuts(JGetString("itsDiscardButton::JXOKToCloseDialog::shortcuts::JXLayout"));

    JXStaticText* text =
        new JXStaticText(JGetString("text::JXOKToCloseDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
    assert( text != NULL );

    JXImageWidget* icon =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
    assert( icon != NULL );

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXOKToCloseDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 170,80, 60,20);
    assert( cancelButton != NULL );

// end JXLayout

	window->SetTitle("Warning");
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
		const Deactivated* info = dynamic_cast(const Deactivated*, &message);
		assert( info != NULL );
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
		const JBoolean ok = Close();
		assert( ok );
		}

	else
		{
		JXUNDialogBase::Receive(sender, message);
		}
}
