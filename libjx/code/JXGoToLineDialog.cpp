/******************************************************************************
 JXGoToLineDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXGoToLineDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXTextCheckbox.h>
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
	const JIndex	maxLine,
	const JBoolean	physicalLineIndexFlag
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsMaxLineCount(maxLine)
{
	BuildWindow(lineIndex, physicalLineIndexFlag);
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
	const JIndex	lineIndex,
	const JBoolean	physicalLineIndexFlag
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 190,120, "");
	assert( window != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 110,90, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXGoToLineDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,90, 60,20);
	assert( cancelButton != NULL );

	itsLineNumber =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 40,20);
	assert( itsLineNumber != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,20, 70,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsPhysicalLineIndexCB =
		new JXTextCheckbox(JGetString("itsPhysicalLineIndexCB::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 150,20);
	assert( itsPhysicalLineIndexCB != NULL );
	itsPhysicalLineIndexCB->SetShortcuts(JGetString("itsPhysicalLineIndexCB::JXGoToLineDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("Go to line");
	SetButtons(okButton, cancelButton);

	itsLineNumber->SetValue(lineIndex);
	itsLineNumber->SetLowerLimit(1);

	itsPhysicalLineIndexCB->SetState(physicalLineIndexFlag);
}

/******************************************************************************
 GetLineIndex

 ******************************************************************************/

JIndex
JXGoToLineDialog::GetLineIndex
	(
	JBoolean* physicalLineIndexFlag
	)
	const
{
	*physicalLineIndexFlag = itsPhysicalLineIndexCB->IsChecked();

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
