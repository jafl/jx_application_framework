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

	JXStaticText* s1Label =
		jnew JXStaticText(JGetString("s1Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 100,20);
	assert( s1Label != NULL );
	s1Label->SetToLabel();

	JXStaticText* s2Label =
		jnew JXStaticText(JGetString("s2Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 100,20);
	assert( s2Label != NULL );
	s2Label->SetFontName("Times");
	s2Label->SetFontSize(8);
	const JFontStyle s2Label_style(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetGreenColor());
	s2Label->SetFontStyle(s2Label_style);
	s2Label->SetToLabel();

	JXStaticText* s3Label =
		jnew JXStaticText(JGetString("s3Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 100,20);
	assert( s3Label != NULL );
	s3Label->SetFontName("Times");
	const JFontStyle s3Label_style(kJTrue, kJTrue, 0, kJFalse, GetColormap()->GetBlackColor());
	s3Label->SetFontStyle(s3Label_style);
	s3Label->SetToLabel();

	JXStaticText* s4Label =
		jnew JXStaticText(JGetString("s4Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 100,20);
	assert( s4Label != NULL );
	s4Label->SetFontSize(8);
	const JFontStyle s4Label_style(kJFalse, kJTrue, 0, kJFalse, GetColormap()->GetBlackColor());
	s4Label->SetFontStyle(s4Label_style);
	s4Label->SetToLabel();

	JXStaticText* f1Label =
		jnew JXStaticText(JGetString("f1Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 80,20);
	assert( f1Label != NULL );
	f1Label->SetToLabel();

	JXStaticText* f2Label =
		jnew JXStaticText(JGetString("f2Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,40, 80,20);
	assert( f2Label != NULL );
	f2Label->SetToLabel();

	JXStaticText* f3Label =
		jnew JXStaticText(JGetString("f3Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,60, 80,20);
	assert( f3Label != NULL );
	f3Label->SetToLabel();

	JXStaticText* f4Label =
		jnew JXStaticText(JGetString("f4Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 80,20);
	assert( f4Label != NULL );
	f4Label->SetToLabel();

	JXStaticText* i2Label =
		jnew JXStaticText(JGetString("i2Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,40, 70,20);
	assert( i2Label != NULL );
	i2Label->SetToLabel();

	JXStaticText* i3Label =
		jnew JXStaticText(JGetString("i3Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,60, 70,20);
	assert( i3Label != NULL );
	i3Label->SetToLabel();

	JXStaticText* i4Label =
		jnew JXStaticText(JGetString("i4Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,80, 70,20);
	assert( i4Label != NULL );
	i4Label->SetToLabel();

	JXStaticText* i1Label =
		jnew JXStaticText(JGetString("i1Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,20, 70,20);
	assert( i1Label != NULL );
	i1Label->SetToLabel();

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
