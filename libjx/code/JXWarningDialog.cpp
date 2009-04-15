/******************************************************************************
 JXWarningDialog.cpp

	BASE CLASS = JXUNDialogBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXWarningDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXImageWidget.h>
#include <JXImage.h>
#include <jGlobals.h>
#include <jAssert.h>

#include <jx_un_warning.xpm>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWarningDialog::JXWarningDialog
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

JXWarningDialog::~JXWarningDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXWarningDialog::BuildWindow
	(
	const JCharacter* message
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 330,110, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* noButton =
        new JXTextButton(JGetString("noButton::JXWarningDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 200,80, 60,20);
    assert( noButton != NULL );
    noButton->SetShortcuts(JGetString("noButton::JXWarningDialog::shortcuts::JXLayout"));

    JXTextButton* yesButton =
        new JXTextButton(JGetString("yesButton::JXWarningDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 70,80, 60,20);
    assert( yesButton != NULL );
    yesButton->SetShortcuts(JGetString("yesButton::JXWarningDialog::shortcuts::JXLayout"));

    JXStaticText* text =
        new JXStaticText(JGetString("text::JXWarningDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 60,20, 250,50);
    assert( text != NULL );

    JXImageWidget* icon =
        new JXImageWidget(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 40,40);
    assert( icon != NULL );

// end JXLayout

	window->SetTitle("Warning");
	SetButtons(yesButton, noButton);
	noButton->SetShortcuts("#N");		// avoid the automatic escape key shortcut

	Init(window, text, message, icon, jx_un_warning);
}
