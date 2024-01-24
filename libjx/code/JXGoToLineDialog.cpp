/******************************************************************************
 JXGoToLineDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXGoToLineDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXIntegerInput.h"
#include "JXTextCheckbox.h"
#include "JXStaticText.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXGoToLineDialog::JXGoToLineDialog
	(
	const JIndex	lineIndex,
	const JIndex	maxLine,
	const bool		physicalLineIndexFlag
	)
	:
	JXModalDialogDirector(),
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
	const bool		physicalLineIndexFlag
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 190,120, JGetString("WindowTitle::JXGoToLineDialog::JXLayout"));

	auto* gotoLineLabel =
		jnew JXStaticText(JGetString("gotoLineLabel::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,20, 70,20);
	gotoLineLabel->SetToLabel(false);

	itsPhysicalLineIndexCB =
		jnew JXTextCheckbox(JGetString("itsPhysicalLineIndexCB::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 150,20);
	itsPhysicalLineIndexCB->SetShortcuts(JGetString("itsPhysicalLineIndexCB::shortcuts::JXGoToLineDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,90, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 109,89, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JXGoToLineDialog::JXLayout"));

	itsLineNumber =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 40,20);
	itsLineNumber->SetLowerLimit(1);

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsLineNumber->SetValue(lineIndex);
	itsPhysicalLineIndexCB->SetState(physicalLineIndexFlag);
}

/******************************************************************************
 GetLineIndex

 ******************************************************************************/

JIndex
JXGoToLineDialog::GetLineIndex
	(
	bool* physicalLineIndexFlag
	)
	const
{
	*physicalLineIndexFlag = itsPhysicalLineIndexCB->IsChecked();

	JInteger lineIndex;
	const bool ok = itsLineNumber->GetValue(&lineIndex);
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
