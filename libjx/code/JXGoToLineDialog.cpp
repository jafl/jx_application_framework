/******************************************************************************
 JXGoToLineDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXGoToLineDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXIntegerInput.h"
#include "JXTextCheckbox.h"
#include "JXStaticText.h"
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
	const bool	physicalLineIndexFlag
	)
	:
	JXDialogDirector(supervisor, true),
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
	const bool	physicalLineIndexFlag
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 190,120, JString::empty);
	assert( window != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 110,90, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXGoToLineDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,90, 60,20);
	assert( cancelButton != nullptr );

	itsLineNumber =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 40,20);
	assert( itsLineNumber != nullptr );

	auto* gotoLineLabel =
		jnew JXStaticText(JGetString("gotoLineLabel::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,20, 70,20);
	assert( gotoLineLabel != nullptr );
	gotoLineLabel->SetToLabel();

	itsPhysicalLineIndexCB =
		jnew JXTextCheckbox(JGetString("itsPhysicalLineIndexCB::JXGoToLineDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 150,20);
	assert( itsPhysicalLineIndexCB != nullptr );
	itsPhysicalLineIndexCB->SetShortcuts(JGetString("itsPhysicalLineIndexCB::JXGoToLineDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXGoToLineDialog"));
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
