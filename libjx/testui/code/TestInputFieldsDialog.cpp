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
#include <jXGlobals.h>
#include <JFontManager.h>
#include <JColorManager.h>
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

	JXWindow* window = jnew JXWindow(this, 620,180, JString::empty);
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

	JXStaticText* stringLabel =
		jnew JXStaticText(JGetString("stringLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 100,20);
	assert( stringLabel != NULL );
	stringLabel->SetToLabel();

	JXStaticText* reqStringLabel =
		jnew JXStaticText(JGetString("reqStringLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 100,20);
	assert( reqStringLabel != NULL );
	reqStringLabel->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	reqStringLabel->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	const JFontStyle reqStringLabel_style(kJTrue, kJFalse, 0, kJFalse, JColorManager::GetGreenColor());
	reqStringLabel->SetFontStyle(reqStringLabel_style);
	reqStringLabel->SetToLabel();

	JXStaticText* max5Label =
		jnew JXStaticText(JGetString("max5Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 100,20);
	assert( max5Label != NULL );
	max5Label->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	const JFontStyle max5Label_style(kJTrue, kJTrue, 0, kJFalse, JColorManager::GetBlackColor());
	max5Label->SetFontStyle(max5Label_style);
	max5Label->SetToLabel();

	JXStaticText* max10Label =
		jnew JXStaticText(JGetString("max10Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 100,20);
	assert( max10Label != NULL );
	max10Label->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	const JFontStyle max10Label_style(kJFalse, kJTrue, 0, kJFalse, JColorManager::GetBlackColor());
	max10Label->SetFontStyle(max10Label_style);
	max10Label->SetToLabel();

	JXStaticText* numberLabel =
		jnew JXStaticText(JGetString("numberLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 80,20);
	assert( numberLabel != NULL );
	numberLabel->SetToLabel();

	JXStaticText* positiveLabel =
		jnew JXStaticText(JGetString("positiveLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,40, 80,20);
	assert( positiveLabel != NULL );
	positiveLabel->SetToLabel();

	JXStaticText* negativeLabel =
		jnew JXStaticText(JGetString("negativeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,60, 80,20);
	assert( negativeLabel != NULL );
	negativeLabel->SetToLabel();

	JXStaticText* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 80,20);
	assert( rangeLabel != NULL );
	rangeLabel->SetToLabel();

	JXStaticText* lowIntLabel =
		jnew JXStaticText(JGetString("lowIntLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,40, 70,20);
	assert( lowIntLabel != NULL );
	lowIntLabel->SetToLabel();

	JXStaticText* highIntLabel =
		jnew JXStaticText(JGetString("highIntLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,60, 70,20);
	assert( highIntLabel != NULL );
	highIntLabel->SetToLabel();

	JXStaticText* intRangeLabel =
		jnew JXStaticText(JGetString("intRangeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,80, 70,20);
	assert( intRangeLabel != NULL );
	intRangeLabel->SetToLabel();

	JXStaticText* intLabel =
		jnew JXStaticText(JGetString("intLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,20, 70,20);
	assert( intLabel != NULL );
	intLabel->SetToLabel();

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

	window->SetTitle(JGetString("WindowTitle::TestInputFieldsDialog"));
	SetButtons(okButton, cancelButton);

	text2->GetText()->SetText(JGetString("Text2::TestInputFieldsDialog"));
	text2->SetIsRequired();
	text3->SetMaxLength(5);
	text4->GetText()->SetText(JGetString("Text4::TestInputFieldsDialog"));
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

	pwInput->GetText()->SetText(JGetString("Password::TestInputFieldsDialog"));
	pwInput->SetHint(JGetString("PasswordHint::TestInputFieldsDialog"));
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
		(JGetUserNotification())->ReportError(JGetString("MinMaxError::TestInputFieldsDialog"));
		itsLowerValue->Focus();
		return kJFalse;
		}

	else
		{
		return kJTrue;
		}
}
