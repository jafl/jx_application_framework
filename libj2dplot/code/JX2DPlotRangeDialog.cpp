/******************************************************************************
 JX2DPlotRangeDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JX2DPlotRangeDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXFloatInput.h>
#include <JXStaticText.h>
#include <JXColormap.h>
#include <JMinMax.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JX2DPlotRangeDialog::JX2DPlotRangeDialog
	(
	JXWindowDirector* supervisor,
	const JFloat xMax,
	const JFloat xMin,
	const JFloat yMax,
	const JFloat yMin
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsClearRangeFlag = kJFalse;

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

	JXWindow* window = new JXWindow(this, 390,120, "");
	assert( window != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 280,90, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JX2DPlotRangeDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,90, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::JX2DPlotRangeDialog::shortcuts::JXLayout"));

	itsXMin =
		new JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,20, 120,20);
	assert( itsXMin != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 70,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,20, 30,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsXMax =
		new JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,20, 120,20);
	assert( itsXMax != NULL );

	itsYMin =
		new JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,50, 120,20);
	assert( itsYMin != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,50, 70,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 220,50, 30,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	itsYMax =
		new JXFloatInput(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 250,50, 120,20);
	assert( itsYMax != NULL );

	itsClearButton =
		new JXTextButton(JGetString("itsClearButton::JX2DPlotRangeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,90, 70,20);
	assert( itsClearButton != NULL );

// end JXLayout

	window->SetTitle("Edit Plot Range");
	SetButtons(okButton, cancelButton);
	ListenTo(itsClearButton);
}

/******************************************************************************
 GetRangeValues (public)

	Returns kJTrue if the range should not be cleared.

 ******************************************************************************/

JBoolean
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
		itsClearRangeFlag = kJTrue;
		EndDialog(kJTrue);
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}
