/******************************************************************************
 SCEditScratchTitleDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCEditScratchTitleDialog.h"

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXInputField.h>

#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCEditScratchTitleDialog::SCEditScratchTitleDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsWindowToEdit = supervisor->GetWindow();
	BuildWindow(itsWindowToEdit->GetTitle());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCEditScratchTitleDialog::~SCEditScratchTitleDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SCEditScratchTitleDialog::BuildWindow
	(
	const JCharacter* title
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 210,110, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 30,75, 60,20);
    assert( cancelButton != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 119,74, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXStaticText* obj1 =
        new JXStaticText("New window title:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 110,20);
    assert( obj1 != NULL );

    itsTitle =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 170,20);
    assert( itsTitle != NULL );

// end JXLayout

	window->SetTitle("Change window title");
	SetButtons(okButton, cancelButton);

	itsTitle->SetText(title);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
SCEditScratchTitleDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	else
		{
		itsWindowToEdit->SetTitle(itsTitle->GetText());
		return kJTrue;
		}
}
