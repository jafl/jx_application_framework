/******************************************************************************
 JX2DPlotScaleDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JX2DPlotScaleDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXFloatInput.h>
#include <JXRadioGroup.h>
#include <JXStaticText.h>
#include <JXTextRadioButton.h>
#include <JXColormap.h>
#include <J2DPlotWidget.h>
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
	const JBoolean xLinear,
	const JFloat yMin,
	const JFloat yMax,
	const JFloat yInc,
	const JBoolean yLinear
	)
	:
	JXDialogDirector(supervisor, kJTrue)
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
	const JBoolean xAxis
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

	JXWindow* window = jnew JXWindow(this, 540,190, "");
	assert( window != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 360,160, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JX2DPlotScaleDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 110,160, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::JX2DPlotScaleDialog::shortcuts::JXLayout"));

	itsXMin =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,20, 120,20);
	assert( itsXMin != NULL );

	JXStaticText* xRangeLabel =
		jnew JXStaticText(JGetString("xRangeLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 70,20);
	assert( xRangeLabel != NULL );
	xRangeLabel->SetToLabel();

	JXStaticText* xToLabel =
		jnew JXStaticText(JGetString("xToLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,20, 30,20);
	assert( xToLabel != NULL );
	xToLabel->SetToLabel();

	itsXMax =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,20, 120,20);
	assert( itsXMax != NULL );

	itsXIncLabel =
		jnew JXStaticText(JGetString("itsXIncLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,50, 70,20);
	assert( itsXIncLabel != NULL );
	itsXIncLabel->SetToLabel();

	itsXInc =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,50, 120,20);
	assert( itsXInc != NULL );

	itsYMin =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,90, 120,20);
	assert( itsYMin != NULL );

	JXStaticText* yRangeLabel =
		jnew JXStaticText(JGetString("yRangeLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,90, 70,20);
	assert( yRangeLabel != NULL );
	yRangeLabel->SetToLabel();

	JXStaticText* yToLabel =
		jnew JXStaticText(JGetString("yToLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,90, 30,20);
	assert( yToLabel != NULL );
	yToLabel->SetToLabel();

	itsYMax =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,90, 120,20);
	assert( itsYMax != NULL );

	itsYIncLabel =
		jnew JXStaticText(JGetString("itsYIncLabel::JX2DPlotScaleDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,120, 70,20);
	assert( itsYIncLabel != NULL );
	itsYIncLabel->SetToLabel();

	itsYInc =
		jnew JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,120, 120,20);
	assert( itsYInc != NULL );

	itsXAxisTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,15, 124,64);
	assert( itsXAxisTypeRG != NULL );

	JXTextRadioButton* xLinearRB =
		jnew JXTextRadioButton(kLinear, JGetString("xLinearRB::JX2DPlotScaleDialog::JXLayout"), itsXAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 100,20);
	assert( xLinearRB != NULL );

	JXTextRadioButton* xLogRB =
		jnew JXTextRadioButton(kLog, JGetString("xLogRB::JX2DPlotScaleDialog::JXLayout"), itsXAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 100,20);
	assert( xLogRB != NULL );

	itsYAxisTypeRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 400,85, 124,64);
	assert( itsYAxisTypeRG != NULL );

	JXTextRadioButton* yLinearRB =
		jnew JXTextRadioButton(kLinear, JGetString("yLinearRB::JX2DPlotScaleDialog::JXLayout"), itsYAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 100,20);
	assert( yLinearRB != NULL );

	JXTextRadioButton* yLogRB =
		jnew JXTextRadioButton(kLog, JGetString("yLogRB::JX2DPlotScaleDialog::JXLayout"), itsYAxisTypeRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 100,20);
	assert( yLogRB != NULL );

// end JXLayout

	window->SetTitle("Edit Plot Scale");
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
	JBoolean*	xLinear,
	JFloat*		yMin,
	JFloat*		yMax,
	JFloat*		yInc,
	JBoolean*	yLinear
	)
{
	JFloat min, max;

	itsXMin->GetValue(&min);
	itsXMax->GetValue(&max);
	itsXInc->GetValue(xInc);

	*xMin    = JMin(max,min);
	*xMax    = JMax(max,min);
	*xLinear = JI2B(itsXAxisTypeRG->GetSelectedItem() == kLinear);

	itsYMin->GetValue(&min);
	itsYMax->GetValue(&max);
	itsYInc->GetValue(yInc);

	*yMin    = JMin(max,min);
	*yMax    = JMax(max,min);
	*yLinear = JI2B(itsYAxisTypeRG->GetSelectedItem() == kLinear);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	If log scale, min and max must be positive.

 ******************************************************************************/

JBoolean
JX2DPlotScaleDialog::OKToDeactivate()
{
JFloat value, min,max;

	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	else if (itsXMin->GetValue(&min) && itsXMax->GetValue(&max) &&
			 min == max)
		{
		(JGetUserNotification())->ReportError("Limits cannot be equal.");
		itsXMin->Focus();
		return kJFalse;
		}
	else if (itsYMin->GetValue(&min) && itsYMax->GetValue(&max) &&
			 min == max)
		{
		(JGetUserNotification())->ReportError("Limits cannot be equal.");
		itsYMin->Focus();
		return kJFalse;
		}

	else if (itsXAxisTypeRG->GetSelectedItem() == kLog &&
			 itsXMin->GetValue(&value) && value <= 0.0)
		{
		(JGetUserNotification())->ReportError("Limits must be positive when using log scale.");
		itsXMin->Focus();
		return kJFalse;
		}
	else if (itsXAxisTypeRG->GetSelectedItem() == kLog &&
			 itsXMax->GetValue(&value) && value <= 0.0)
		{
		(JGetUserNotification())->ReportError("Limits must be positive when using log scale.");
		itsXMax->Focus();
		return kJFalse;
		}

	else if (itsYAxisTypeRG->GetSelectedItem() == kLog &&
			 itsYMin->GetValue(&value) && value <= 0.0)
		{
		(JGetUserNotification())->ReportError("Limits must be positive when using log scale.");
		itsYMin->Focus();
		return kJFalse;
		}
	else if (itsYAxisTypeRG->GetSelectedItem() == kLog &&
			 itsYMax->GetValue(&value) && value <= 0.0)
		{
		(JGetUserNotification())->ReportError("Limits must be positive when using log scale.");
		itsYMax->Focus();
		return kJFalse;
		}

	else
		{
		return kJTrue;
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
	const JIndex type    = itsXAxisTypeRG->GetSelectedItem();
	JXColormap* colormap = GetColormap();

	if (type == kLinear)
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
	const JIndex type    = itsYAxisTypeRG->GetSelectedItem();
	JXColormap* colormap = GetColormap();

	if (type == kLinear)
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
