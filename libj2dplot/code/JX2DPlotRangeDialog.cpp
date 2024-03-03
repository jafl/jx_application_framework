/******************************************************************************
 JX2DPlotRangeDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JX2DPlotRangeDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DPlotRangeDialog::JX2DPlotRangeDialog
	(
	const JFloat xMax,
	const JFloat xMin,
	const JFloat yMax,
	const JFloat yMin
	)
	:
	JXModalDialogDirector()
{
	itsClearRangeFlag = false;

	BuildWindow();
	itsXMax->SetValue(xMax);
	itsXMin->SetValue(xMin);
	itsYMax->SetValue(yMax);
	itsYMin->SetValue(yMin);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DPlotRangeDialog::~JX2DPlotRangeDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JX2DPlotRangeDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 390,120, JGetString("WindowTitle::JX2DPlotRangeDialog::JXLayout"));

	auto* xRangeLabel =
		jnew JXStaticText(JGetString("xRangeLabel::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 70,20);
	xRangeLabel->SetToLabel(false);

	auto* xToLabel =
		jnew JXStaticText(JGetString("xToLabel::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,20, 30,20);
	xToLabel->SetToLabel(false);

	auto* yRangeLabel =
		jnew JXStaticText(JGetString("yRangeLabel::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 70,20);
	yRangeLabel->SetToLabel(false);

	auto* yToLabel =
		jnew JXStaticText(JGetString("yToLabel::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,50, 30,20);
	yToLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,90, 70,20);
	cancelButton->SetShortcuts(JGetString("cancelButton::shortcuts::JX2DPlotRangeDialog::JXLayout"));

	itsClearButton =
		jnew JXTextButton(JGetString("itsClearButton::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,90, 70,20);

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,90, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JX2DPlotRangeDialog::JXLayout"));

	itsXMin =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,20, 120,20);
	itsXMin->SetIsRequired(false);

	itsXMax =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,20, 120,20);
	itsXMax->SetIsRequired(false);

	itsYMin =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,50, 120,20);
	itsYMin->SetIsRequired(false);

	itsYMax =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,50, 120,20);
	itsYMax->SetIsRequired(false);

// end JXLayout

	SetButtons(okButton, cancelButton);
	ListenTo(itsClearButton);
}

/******************************************************************************
 GetRangeValues (public)

	Returns true if the range should not be cleared.

 ******************************************************************************/

bool
JX2DPlotRangeDialog::GetRangeValues
	(
	JFloat* xMax,
	JFloat* xMin,
	JFloat* yMax,
	JFloat* yMin
	)
{
	JFloat min;
	JFloat max;
	itsXMax->GetValue(&max);
	itsXMin->GetValue(&min);
	*xMax = JMax(max,min);
	*xMin = JMin(max,min);
	itsYMax->GetValue(&max);
	itsYMin->GetValue(&min);
	*yMax = JMax(max,min);
	*yMin = JMin(max,min);
	return !itsClearRangeFlag;
}

/*******************************************************************************
 Receive

 ******************************************************************************/

void
JX2DPlotRangeDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsClearButton && message.Is(JXButton::kPushed))
	{
		itsClearRangeFlag = true;
		EndDialog(true);
	}
	else
	{
		JXModalDialogDirector::Receive(sender, message);
	}
}
