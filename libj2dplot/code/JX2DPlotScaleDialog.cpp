/******************************************************************************
 JX2DPlotScaleDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JX2DPlotScaleDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXTextRadioButton.h>
#include <jx-af/jx/JXColorManager.h>
#include "J2DPlotWidget.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/JMinMax.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DPlotScaleDialog::JX2DPlotScaleDialog
	(
	const JFloat xMin,
	const JFloat xMax,
	const JFloat xInc,
	const bool xLinear,
	const JFloat yMin,
	const JFloat yMax,
	const JFloat yInc,
	const bool yLinear,
	const bool editXAxis
	)
	:
	JXModalDialogDirector(),
	itsEditXAxisFlag(editXAxis)
{
	BuildWindow();

	itsXMin->SetValue(xMin);
	itsXMax->SetValue(xMax);
	itsXInc->SetValue(xInc);
	if (xLinear)
	{
		itsXAxisTypeRG->SelectItem(kLinear);
	}
	else
	{
		itsXAxisTypeRG->SelectItem(kLog);
	}

	itsYMin->SetValue(yMin);
	itsYMax->SetValue(yMax);
	itsYInc->SetValue(yInc);
	if (yLinear)
	{
		itsYAxisTypeRG->SelectItem(kLinear);
	}
	else
	{
		itsYAxisTypeRG->SelectItem(kLog);
	}

	ListenTo(itsXAxisTypeRG);
	ListenTo(itsYAxisTypeRG);
	AdjustXScaleActivation();
	AdjustYScaleActivation();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DPlotScaleDialog::~JX2DPlotScaleDialog()
{
}

/******************************************************************************
 Activate (virtual)

 ******************************************************************************/

void
JX2DPlotScaleDialog::Activate()
{
	JXModalDialogDirector::Activate();

	if (IsActive() && !itsEditXAxisFlag)
	{
		itsYMin->Focus();
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JX2DPlotScaleDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 540,190, JGetString("WindowTitle::JX2DPlotScaleDialog::JXLayout"));

	itsXAxisTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,15, 124,64);

	auto* xRangeLabel =
		jnew JXStaticText(JGetString("xRangeLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 70,20);
	xRangeLabel->SetToLabel(false);

	auto* xToLabel =
		jnew JXStaticText(JGetString("xToLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,20, 30,20);
	xToLabel->SetToLabel(false);

	auto* xLinearRB =
		jnew JXTextRadioButton(kLinear, JGetString("xLinearRB::JX2DPlotScaleDialog::JXLayout"), itsXAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 100,20);
	assert( xLinearRB != nullptr );

	auto* xLogRB =
		jnew JXTextRadioButton(kLog, JGetString("xLogRB::JX2DPlotScaleDialog::JXLayout"), itsXAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 100,20);
	assert( xLogRB != nullptr );

	itsXIncLabel =
		jnew JXStaticText(JGetString("itsXIncLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,50, 70,20);
	itsXIncLabel->SetToLabel(false);

	itsYAxisTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,85, 124,64);

	auto* yRangeLabel =
		jnew JXStaticText(JGetString("yRangeLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 70,20);
	yRangeLabel->SetToLabel(false);

	auto* yToLabel =
		jnew JXStaticText(JGetString("yToLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,90, 30,20);
	yToLabel->SetToLabel(false);

	auto* yLinearRB =
		jnew JXTextRadioButton(kLinear, JGetString("yLinearRB::JX2DPlotScaleDialog::JXLayout"), itsYAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 100,20);
	assert( yLinearRB != nullptr );

	auto* yLogRB =
		jnew JXTextRadioButton(kLog, JGetString("yLogRB::JX2DPlotScaleDialog::JXLayout"), itsYAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 100,20);
	assert( yLogRB != nullptr );

	itsYIncLabel =
		jnew JXStaticText(JGetString("itsYIncLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,120, 70,20);
	itsYIncLabel->SetToLabel(false);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,160, 70,20);
	cancelButton->SetShortcuts(JGetString("cancelButton::shortcuts::JX2DPlotScaleDialog::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 360,160, 70,20);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::JX2DPlotScaleDialog::JXLayout"));

	itsXMin =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,20, 120,20);
	itsXMin->SetIsRequired(false);

	itsXMax =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,20, 120,20);
	itsXMax->SetIsRequired(false);

	itsXInc =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,50, 120,20);
	itsXInc->SetIsRequired(false);
	itsXInc->SetLowerLimit(0);

	itsYMin =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,90, 120,20);
	itsYMin->SetIsRequired(false);

	itsYMax =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,90, 120,20);
	itsYMax->SetIsRequired(false);

	itsYInc =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,120, 120,20);
	itsYInc->SetIsRequired(false);
	itsYInc->SetLowerLimit(0);

// end JXLayout

	SetButtons(okButton, cancelButton);
}

/******************************************************************************
 GetScaleValues

 ******************************************************************************/

void
JX2DPlotScaleDialog::GetScaleValues
	(
	JFloat*		xMin,
	JFloat*		xMax,
	JFloat*		xInc,
	bool*	xLinear,
	JFloat*		yMin,
	JFloat*		yMax,
	JFloat*		yInc,
	bool*	yLinear
	)
{
	JFloat min, max;

	itsXMin->GetValue(&min);
	itsXMax->GetValue(&max);
	itsXInc->GetValue(xInc);

	*xMin    = JMin(max,min);
	*xMax    = JMax(max,min);
	*xLinear = itsXAxisTypeRG->GetSelectedItem() == kLinear;

	itsYMin->GetValue(&min);
	itsYMax->GetValue(&max);
	itsYInc->GetValue(yInc);

	*yMin    = JMin(max,min);
	*yMax    = JMax(max,min);
	*yLinear = itsYAxisTypeRG->GetSelectedItem() == kLinear;
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	If log scale, min and max must be positive.

 ******************************************************************************/

bool
JX2DPlotScaleDialog::OKToDeactivate()
{
JFloat value, min,max;

	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	else if (itsXMin->GetValue(&min) && itsXMax->GetValue(&max) &&
			 min == max)
	{
		JGetUserNotification()->ReportError(JGetString("LimitsMustBeDifferent::JX2DPlotScaleDialog"));
		itsXMin->Focus();
		return false;
	}
	else if (itsYMin->GetValue(&min) && itsYMax->GetValue(&max) &&
			 min == max)
	{
		JGetUserNotification()->ReportError(JGetString("LimitsMustBeDifferent::JX2DPlotScaleDialog"));
		itsYMin->Focus();
		return false;
	}

	else if (itsXAxisTypeRG->GetSelectedItem() == kLog &&
			 itsXMin->GetValue(&value) && value <= 0.0)
	{
		JGetUserNotification()->ReportError(JGetString("PositiveLimitsForLog::JX2DPlotScaleDialog"));
		itsXMin->Focus();
		return false;
	}
	else if (itsXAxisTypeRG->GetSelectedItem() == kLog &&
			 itsXMax->GetValue(&value) && value <= 0.0)
	{
		JGetUserNotification()->ReportError(JGetString("PositiveLimitsForLog::JX2DPlotScaleDialog"));
		itsXMax->Focus();
		return false;
	}

	else if (itsYAxisTypeRG->GetSelectedItem() == kLog &&
			 itsYMin->GetValue(&value) && value <= 0.0)
	{
		JGetUserNotification()->ReportError(JGetString("PositiveLimitsForLog::JX2DPlotScaleDialog"));
		itsYMin->Focus();
		return false;
	}
	else if (itsYAxisTypeRG->GetSelectedItem() == kLog &&
			 itsYMax->GetValue(&value) && value <= 0.0)
	{
		JGetUserNotification()->ReportError(JGetString("PositiveLimitsForLog::JX2DPlotScaleDialog"));
		itsYMax->Focus();
		return false;
	}

	else
	{
		return true;
	}
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
JX2DPlotScaleDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsXAxisTypeRG && message.Is(JXRadioGroup::kSelectionChanged))
	{
		AdjustXScaleActivation();
	}

	else if (sender == itsYAxisTypeRG && message.Is(JXRadioGroup::kSelectionChanged))
	{
		AdjustYScaleActivation();
	}

	else
	{
		JXModalDialogDirector::Receive(sender,message);
	}
}

/******************************************************************************
 AdjustXScaleActivation (private)

 ******************************************************************************/

void
JX2DPlotScaleDialog::AdjustXScaleActivation()
{
	if (itsXAxisTypeRG->GetSelectedItem() == kLinear)
	{
		itsXIncLabel->Show();
		itsXInc->Show();
	}
	else
	{
		itsXIncLabel->Hide();
		itsXInc->Hide();
	}
}

/******************************************************************************
 AdjustYScaleActivation (private)

 ******************************************************************************/

void
JX2DPlotScaleDialog::AdjustYScaleActivation()
{
	if (itsYAxisTypeRG->GetSelectedItem() == kLinear)
	{
		itsYIncLabel->Show();
		itsYInc->Show();
	}
	else
	{
		itsYIncLabel->Hide();
		itsYInc->Hide();
	}
}
