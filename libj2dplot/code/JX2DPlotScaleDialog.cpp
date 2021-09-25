/******************************************************************************
 JX2DPlotScaleDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "JX2DPlotScaleDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXFloatInput.h>
#include <JXRadioGroup.h>
#include <JXStaticText.h>
#include <JXTextRadioButton.h>
#include <JXColorManager.h>
#include "J2DPlotWidget.h"
#include <jGlobals.h>
#include <JMinMax.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DPlotScaleDialog::JX2DPlotScaleDialog
	(
	JXWindowDirector* supervisor,
	const JFloat xMin,
	const JFloat xMax,
	const JFloat xInc,
	const bool xLinear,
	const JFloat yMin,
	const JFloat yMax,
	const JFloat yInc,
	const bool yLinear
	)
	:
	JXDialogDirector(supervisor, true)
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
 EditXAxis

 ******************************************************************************/

void
JX2DPlotScaleDialog::EditXAxis
	(
	const bool xAxis
	)
{
	if (!xAxis)
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

	auto* window = jnew JXWindow(this, 540,190, JString::empty);
	assert( window != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 360,160, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JX2DPlotScaleDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,160, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::JX2DPlotScaleDialog::shortcuts::JXLayout"));

	itsXMin =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,20, 120,20);
	assert( itsXMin != nullptr );

	auto* xRangeLabel =
		jnew JXStaticText(JGetString("xRangeLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 70,20);
	assert( xRangeLabel != nullptr );
	xRangeLabel->SetToLabel();

	auto* xToLabel =
		jnew JXStaticText(JGetString("xToLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,20, 30,20);
	assert( xToLabel != nullptr );
	xToLabel->SetToLabel();

	itsXMax =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,20, 120,20);
	assert( itsXMax != nullptr );

	itsXIncLabel =
		jnew JXStaticText(JGetString("itsXIncLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,50, 70,20);
	assert( itsXIncLabel != nullptr );
	itsXIncLabel->SetToLabel();

	itsXInc =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,50, 120,20);
	assert( itsXInc != nullptr );

	itsYMin =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,90, 120,20);
	assert( itsYMin != nullptr );

	auto* yRangeLabel =
		jnew JXStaticText(JGetString("yRangeLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 70,20);
	assert( yRangeLabel != nullptr );
	yRangeLabel->SetToLabel();

	auto* yToLabel =
		jnew JXStaticText(JGetString("yToLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,90, 30,20);
	assert( yToLabel != nullptr );
	yToLabel->SetToLabel();

	itsYMax =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,90, 120,20);
	assert( itsYMax != nullptr );

	itsYIncLabel =
		jnew JXStaticText(JGetString("itsYIncLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,120, 70,20);
	assert( itsYIncLabel != nullptr );
	itsYIncLabel->SetToLabel();

	itsYInc =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,120, 120,20);
	assert( itsYInc != nullptr );

	itsXAxisTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,15, 124,64);
	assert( itsXAxisTypeRG != nullptr );

	auto* xLinearRB =
		jnew JXTextRadioButton(kLinear, JGetString("xLinearRB::JX2DPlotScaleDialog::JXLayout"), itsXAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 100,20);
	assert( xLinearRB != nullptr );

	auto* xLogRB =
		jnew JXTextRadioButton(kLog, JGetString("xLogRB::JX2DPlotScaleDialog::JXLayout"), itsXAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 100,20);
	assert( xLogRB != nullptr );

	itsYAxisTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,85, 124,64);
	assert( itsYAxisTypeRG != nullptr );

	auto* yLinearRB =
		jnew JXTextRadioButton(kLinear, JGetString("yLinearRB::JX2DPlotScaleDialog::JXLayout"), itsYAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 100,20);
	assert( yLinearRB != nullptr );

	auto* yLogRB =
		jnew JXTextRadioButton(kLog, JGetString("yLogRB::JX2DPlotScaleDialog::JXLayout"), itsYAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 100,20);
	assert( yLogRB != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JX2DPlotScaleDialog"));
	SetButtons(okButton, cancelButton);

	itsXInc->SetLowerLimit(0.0);
	itsYInc->SetLowerLimit(0.0);
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

	if (!JXDialogDirector::OKToDeactivate())
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
		JXDialogDirector::Receive(sender,message);
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
