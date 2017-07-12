/******************************************************************************
 TestInputFieldsDialog.cpp

	BASE CLASS = JXDialogDirector

	Written by John Lindal.

 ******************************************************************************/

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

	JXWindow* window = jnew JXWindow(this, 620,180, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,150, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,150, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::TestInputFieldsDialog::shortcuts::JXLayout"));

	JXInputField* text1 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 180,20);
	assert( text1 != NULL );

	JXInputField* text2 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,40, 180,20);
	assert( text2 != NULL );

	JXInputField* text3 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,60, 180,20);
	assert( text3 != NULL );

	JXInputField* text4 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,80, 180,20);
	assert( text4 != NULL );

	JXFloatInput* f1 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,20, 90,20);
	assert( f1 != NULL );

	JXFloatInput* f2 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,40, 90,20);
	assert( f2 != NULL );

	JXFloatInput* f3 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,60, 90,20);
	assert( f3 != NULL );

	JXFloatInput* f4 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,80, 90,20);
	assert( f4 != NULL );

	JXIntegerInput* i1 =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,20, 50,20);
	assert( i1 != NULL );

	itsLowerValue =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,40, 50,20);
	assert( itsLowerValue != NULL );

	itsUpperValue =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,60, 50,20);
	assert( itsUpperValue != NULL );

	JXIntegerInput* i4 =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,80, 50,20);
	assert( i4 != NULL );

	JXStaticText* label1 =
		jnew JXStaticText(JGetString("label1::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 100,20);
	assert( label1 != NULL );
	label1->SetToLabel();

	JXStaticText* label2 =
		jnew JXStaticText(JGetString("label2::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 100,20);
	assert( label2 != NULL );
	label2->SetFontName("Times");
	label2->SetFontSize(8);
	const JFontStyle label2_style(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetGreenColor());
	label2->SetFontStyle(label2_style);
	label2->SetToLabel();

	JXStaticText* label3 =
		jnew JXStaticText(JGetString("label3::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 100,20);
	assert( label3 != NULL );
	label3->SetFontName("Times");
	const JFontStyle label3_style(kJTrue, kJTrue, 0, kJFalse, GetColormap()->GetBlackColor());
	label3->SetFontStyle(label3_style);
	label3->SetToLabel();

	JXStaticText* label4 =
		jnew JXStaticText(JGetString("label4::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 100,20);
	assert( label4 != NULL );
	label4->SetFontSize(8);
	const JFontStyle label4_style(kJFalse, kJTrue, 0, kJFalse, GetColormap()->GetBlackColor());
	label4->SetFontStyle(label4_style);
	label4->SetToLabel();

	JXStaticText* labelf1 =
		jnew JXStaticText(JGetString("labelf1::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 80,20);
	assert( labelf1 != NULL );
	labelf1->SetToLabel();

	JXStaticText* labelf2 =
		jnew JXStaticText(JGetString("labelf2::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,40, 80,20);
	assert( labelf2 != NULL );
	labelf2->SetToLabel();

	JXStaticText* labelf3 =
		jnew JXStaticText(JGetString("labelf3::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,60, 80,20);
	assert( labelf3 != NULL );
	labelf3->SetToLabel();

	JXStaticText* labelf4 =
		jnew JXStaticText(JGetString("labelf4::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 80,20);
	assert( labelf4 != NULL );
	labelf4->SetToLabel();

	JXStaticText* labeli2 =
		jnew JXStaticText(JGetString("labeli2::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,40, 70,20);
	assert( labeli2 != NULL );
	labeli2->SetToLabel();

	JXStaticText* labeli3 =
		jnew JXStaticText(JGetString("labeli3::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,60, 70,20);
	assert( labeli3 != NULL );
	labeli3->SetToLabel();

	JXStaticText* labeli4 =
		jnew JXStaticText(JGetString("labeli4::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,80, 70,20);
	assert( labeli4 != NULL );
	labeli4->SetToLabel();

	JXStaticText* labeli1 =
		jnew JXStaticText(JGetString("labeli1::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,20, 70,20);
	assert( labeli1 != NULL );
	labeli1->SetToLabel();

	JXPasswordInput* pwInput =
		jnew JXPasswordInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,110, 180,20);
	assert( pwInput != NULL );

	JXStaticText* pwLabel =
		jnew JXStaticText(JGetString("pwLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 100,20);
	assert( pwLabel != NULL );
	pwLabel->SetToLabel();

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
