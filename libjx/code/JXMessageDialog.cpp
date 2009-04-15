/******************************************************************************
 JXMessageDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXMessageDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_un_message.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXMessageDialog::JXMessageDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	message
	)
	:
	JXUNDialogBase(supervisor)
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
	const JCharacter* message
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 330,110, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXMessageDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 139,79, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXMessageDialog::shortcuts::JXLayout"));

    JXStaticText* text =
        new JXStaticText(JGetString("text::JXMessageDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
    assert( text != NULL );

    JXImageWidget* icon =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
    assert( icon != NULL );

// end JXLayout

	window->SetTitle("Message");
	SetButtons(okButton, NULL);

	Init(window, text, message, icon, jx_un_message);
}
