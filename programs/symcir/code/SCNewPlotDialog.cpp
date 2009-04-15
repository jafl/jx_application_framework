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
    SetWindow(window);

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 60,90, 60,20);
    assert( cancelButton != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 179,89, 62,22);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXStaticText* obj1 =
        new JXStaticText("Curve name:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 80,20);
    assert( obj1 != NULL );

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

    JXStaticText* obj2 =
        new JXStaticText("Frequency range (Hz):", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,50, 140,20);
    assert( obj2 != NULL );

    JXStaticText* obj3 =
        new JXStaticText("to", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 210,50, 20,20);
    assert( obj3 != NULL );

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
