/******************************************************************************
 TestInputFieldsDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestInputFieldsDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXFloatInput.h>
#include <JXIntegerInput.h>
#include <JXPasswordInput.h>
#include <JXStaticText.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestInputFieldsDialog::TestInputFieldsDialog
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

TestInputFieldsDialog::~TestInputFieldsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestInputFieldsDialog::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 620,180, "");
    assert( window != NULL );

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,150, 70,20);
    assert( cancelButton != NULL );

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 390,150, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::TestInputFieldsDialog::shortcuts::JXLayout"));

    JXInputField* text1 =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 180,20);
    assert( text1 != NULL );

    JXInputField* text2 =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,40, 180,20);
    assert( text2 != NULL );

    JXInputField* text3 =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,60, 180,20);
    assert( text3 != NULL );

    JXInputField* text4 =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,80, 180,20);
    assert( text4 != NULL );

    JXFloatInput* f1 =
        new JXFloatInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 380,20, 90,20);
    assert( f1 != NULL );

    JXFloatInput* f2 =
        new JXFloatInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 380,40, 90,20);
    assert( f2 != NULL );

    JXFloatInput* f3 =
        new JXFloatInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 380,60, 90,20);
    assert( f3 != NULL );

    JXFloatInput* f4 =
        new JXFloatInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 380,80, 90,20);
    assert( f4 != NULL );

    JXIntegerInput* i1 =
        new JXIntegerInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 550,20, 50,20);
    assert( i1 != NULL );

    itsLowerValue =
        new JXIntegerInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 550,40, 50,20);
    assert( itsLowerValue != NULL );

    itsUpperValue =
        new JXIntegerInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 550,60, 50,20);
    assert( itsUpperValue != NULL );

    JXIntegerInput* i4 =
        new JXIntegerInput(window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 550,80, 50,20);
    assert( i4 != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 100,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    JXStaticText* obj2_JXLayout =
        new JXStaticText(JGetString("obj2_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 100,20);
    assert( obj2_JXLayout != NULL );
    obj2_JXLayout->SetFontName("Times");
    obj2_JXLayout->SetFontSize(8);
    const JFontStyle obj2_JXLayout_style(kJTrue, kJFalse, 0, kJFalse, (GetColormap())->GetGreenColor());
    obj2_JXLayout->SetFontStyle(obj2_JXLayout_style);
    obj2_JXLayout->SetToLabel();

    JXStaticText* obj3_JXLayout =
        new JXStaticText(JGetString("obj3_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 100,20);
    assert( obj3_JXLayout != NULL );
    obj3_JXLayout->SetFontName("Times");
    const JFontStyle obj3_JXLayout_style(kJTrue, kJTrue, 0, kJFalse, (GetColormap())->GetBlackColor());
    obj3_JXLayout->SetFontStyle(obj3_JXLayout_style);
    obj3_JXLayout->SetToLabel();

    JXStaticText* obj4_JXLayout =
        new JXStaticText(JGetString("obj4_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 100,20);
    assert( obj4_JXLayout != NULL );
    obj4_JXLayout->SetFontSize(8);
    const JFontStyle obj4_JXLayout_style(kJFalse, kJTrue, 0, kJFalse, (GetColormap())->GetBlackColor());
    obj4_JXLayout->SetFontStyle(obj4_JXLayout_style);
    obj4_JXLayout->SetToLabel();

    JXStaticText* obj5_JXLayout =
        new JXStaticText(JGetString("obj5_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 80,20);
    assert( obj5_JXLayout != NULL );
    obj5_JXLayout->SetToLabel();

    JXStaticText* obj6_JXLayout =
        new JXStaticText(JGetString("obj6_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,40, 80,20);
    assert( obj6_JXLayout != NULL );
    obj6_JXLayout->SetToLabel();

    JXStaticText* obj7_JXLayout =
        new JXStaticText(JGetString("obj7_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,60, 80,20);
    assert( obj7_JXLayout != NULL );
    obj7_JXLayout->SetToLabel();

    JXStaticText* obj8_JXLayout =
        new JXStaticText(JGetString("obj8_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 80,20);
    assert( obj8_JXLayout != NULL );
    obj8_JXLayout->SetToLabel();

    JXStaticText* obj9_JXLayout =
        new JXStaticText(JGetString("obj9_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 480,40, 70,20);
    assert( obj9_JXLayout != NULL );
    obj9_JXLayout->SetToLabel();

    JXStaticText* obj10_JXLayout =
        new JXStaticText(JGetString("obj10_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 480,60, 70,20);
    assert( obj10_JXLayout != NULL );
    obj10_JXLayout->SetToLabel();

    JXStaticText* obj11_JXLayout =
        new JXStaticText(JGetString("obj11_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 480,80, 70,20);
    assert( obj11_JXLayout != NULL );
    obj11_JXLayout->SetToLabel();

    JXStaticText* obj12_JXLayout =
        new JXStaticText(JGetString("obj12_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedRight, JXWidget::kFixedTop, 480,20, 70,20);
    assert( obj12_JXLayout != NULL );
    obj12_JXLayout->SetToLabel();

    JXPasswordInput* pwInput =
        new JXPasswordInput(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 110,110, 180,20);
    assert( pwInput != NULL );

    JXStaticText* obj13_JXLayout =
        new JXStaticText(JGetString("obj13_JXLayout::TestInputFieldsDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 100,20);
    assert( obj13_JXLayout != NULL );
    obj13_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Testing Input Fields");
	SetButtons(okButton, cancelButton);

	text2->SetText("required");
	text2->SetIsRequired();
	text3->SetMaxLength(5);
	text4->SetText("max 10");
	text4->SetIsRequired();
	text4->SetMaxLength(10);

	f2->SetLowerLimit(-3.2);
	f3->SetUpperLimit(-3.2);
	f4->SetLimits(2.1, 10.3);

	itsLowerValue->SetLowerLimit(-3);
	itsUpperValue->SetUpperLimit(8);
	i4->SetLimits(-2, 24);

	// always set initial values so they don't generate errors

	itsLowerValue->SetValue(-1);

	// never store real passwords as plaintext!

	pwInput->SetText("password");
	pwInput->SetHint("For security, the actual password is not displayed");
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Demonstrates how to add extra restrictions to input fields.

 ******************************************************************************/

JBoolean
TestInputFieldsDialog::OKToDeactivate()
{
JInteger v1,v2;

	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	else if (itsLowerValue->GetValue(&v1) &&
			 itsUpperValue->GetValue(&v2) &&
			 v1 >= v2)
		{
		(JGetUserNotification())->ReportError("Low must be less than high.");
		itsLowerValue->Focus();
		return kJFalse;
		}

	else
		{
		return kJTrue;
		}
}
