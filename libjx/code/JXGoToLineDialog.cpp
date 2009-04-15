/******************************************************************************
 JXGoToLineDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXGoToLineDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGoToLineDialog::JXGoToLineDialog
	(
	JXDirector*		supervisor,
	const JIndex	lineIndex,
	const JIndex	maxLine
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsMaxLineCount( maxLine )
{
	BuildWindow(lineIndex);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXGoToLineDialog::~JXGoToLineDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXGoToLineDialog::BuildWindow
	(
	const JIndex lineIndex
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 190,90, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::JXGoToLineDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 109,59, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::JXGoToLineDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::JXGoToLineDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,60, 60,20);
    assert( cancelButton != NULL );

    itsLineNumber =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 40,20);
    assert( itsLineNumber != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::JXGoToLineDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,20, 70,20);
    assert( obj1_JXLayout != NULL );

// end JXLayout

	window->SetTitle("Go to line");
	SetButtons(okButton, cancelButton);

	itsLineNumber->SetValue(lineIndex);
	itsLineNumber->SetLowerLimit(1);
}

/******************************************************************************
 GetLineIndex

 ******************************************************************************/

JIndex
JXGoToLineDialog::GetLineIndex()
	const
{
	JInteger lineIndex;
	const JBoolean ok = itsLineNumber->GetValue(&lineIndex);
	assert( ok );

	if (((JSize) lineIndex) <= itsMaxLineCount)
		{
		return lineIndex;
		}
	else
		{
		return itsMaxLineCount;
		}
}
