/******************************************************************************
 SCNewPlotDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "SCNewPlotDialog.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXInputField.h>
#include <JXFloatInput.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCNewPlotDialog::SCNewPlotDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCNewPlotDialog::~SCNewPlotDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SCNewPlotDialog::BuildWindow()
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 300,120, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SCNewPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 60,90, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SCNewPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 180,90, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SCNewPlotDialog::shortcuts::JXLayout"));

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::SCNewPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 80,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsCurveName =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,20, 180,20);
	assert( itsCurveName != NULL );

	itsMinValue =
		new JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,50, 50,20);
	assert( itsMinValue != NULL );

	itsMaxValue =
		new JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,50, 50,20);
	assert( itsMaxValue != NULL );

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::SCNewPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,50, 140,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::SCNewPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 210,50, 20,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("New plot");
	SetButtons(okButton, cancelButton);

	itsCurveName->SetText("New curve");
	itsCurveName->SetIsRequired();

	itsMinValue->SetValue(1.0);
	itsMinValue->SetLowerLimit(0.0);

	itsMaxValue->SetValue(1000.0);
	itsMaxValue->SetLowerLimit(0.0);
}

/******************************************************************************
 GetSettings

 ******************************************************************************/

void
SCNewPlotDialog::GetSettings
	(
	JString*	curveName,
	JFloat*		fMin,
	JFloat*		fMax
	)
	const
{
	*curveName = itsCurveName->GetText();

	JBoolean ok = itsMinValue->GetValue(fMin);
	assert( ok );

	ok = itsMaxValue->GetValue(fMax);
	assert( ok );
}
