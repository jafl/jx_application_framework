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
#include <JXCharInput.h>
#include <JXPathInput.h>
#include <JXFileInput.h>
#include <JXStaticText.h>
#include <jXGlobals.h>
#include <JFontManager.h>
#include <JColorManager.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestInputFieldsDialog::TestInputFieldsDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, true)
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

	JXWindow* window = jnew JXWindow(this, 620,210, JString::empty);
	assert( window != nullptr );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,180, 70,20);
	assert( cancelButton != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,180, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::TestInputFieldsDialog::shortcuts::JXLayout"));

	JXInputField* text1 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 180,20);
	assert( text1 != nullptr );

	JXInputField* text2 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,40, 180,20);
	assert( text2 != nullptr );

	JXInputField* text3 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,60, 180,20);
	assert( text3 != nullptr );

	JXInputField* text4 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,80, 180,20);
	assert( text4 != nullptr );

	JXFloatInput* f1 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,20, 90,20);
	assert( f1 != nullptr );

	JXFloatInput* f2 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,40, 90,20);
	assert( f2 != nullptr );

	JXFloatInput* f3 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,60, 90,20);
	assert( f3 != nullptr );

	JXFloatInput* f4 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,80, 90,20);
	assert( f4 != nullptr );

	JXIntegerInput* i1 =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,20, 50,20);
	assert( i1 != nullptr );

	itsLowerValue =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,40, 50,20);
	assert( itsLowerValue != nullptr );

	itsUpperValue =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,60, 50,20);
	assert( itsUpperValue != nullptr );

	JXIntegerInput* i4 =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,80, 50,20);
	assert( i4 != nullptr );

	JXStaticText* stringLabel =
		jnew JXStaticText(JGetString("stringLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 100,20);
	assert( stringLabel != nullptr );
	stringLabel->SetToLabel();

	JXStaticText* reqStringLabel =
		jnew JXStaticText(JGetString("reqStringLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 100,20);
	assert( reqStringLabel != nullptr );
	reqStringLabel->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	reqStringLabel->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	const JFontStyle reqStringLabel_style(true, false, 0, false, JColorManager::GetGreenColor());
	reqStringLabel->SetFontStyle(reqStringLabel_style);
	reqStringLabel->SetToLabel();

	JXStaticText* max5Label =
		jnew JXStaticText(JGetString("max5Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 100,20);
	assert( max5Label != nullptr );
	max5Label->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	const JFontStyle max5Label_style(true, true, 0, false, JColorManager::GetBlackColor());
	max5Label->SetFontStyle(max5Label_style);
	max5Label->SetToLabel();

	JXStaticText* max10Label =
		jnew JXStaticText(JGetString("max10Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 100,20);
	assert( max10Label != nullptr );
	max10Label->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	const JFontStyle max10Label_style(false, true, 0, false, JColorManager::GetBlackColor());
	max10Label->SetFontStyle(max10Label_style);
	max10Label->SetToLabel();

	JXStaticText* numberLabel =
		jnew JXStaticText(JGetString("numberLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 80,20);
	assert( numberLabel != nullptr );
	numberLabel->SetToLabel();

	JXStaticText* positiveLabel =
		jnew JXStaticText(JGetString("positiveLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,40, 80,20);
	assert( positiveLabel != nullptr );
	positiveLabel->SetToLabel();

	JXStaticText* negativeLabel =
		jnew JXStaticText(JGetString("negativeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,60, 80,20);
	assert( negativeLabel != nullptr );
	negativeLabel->SetToLabel();

	JXStaticText* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 80,20);
	assert( rangeLabel != nullptr );
	rangeLabel->SetToLabel();

	JXStaticText* lowIntLabel =
		jnew JXStaticText(JGetString("lowIntLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,40, 70,20);
	assert( lowIntLabel != nullptr );
	lowIntLabel->SetToLabel();

	JXStaticText* highIntLabel =
		jnew JXStaticText(JGetString("highIntLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,60, 70,20);
	assert( highIntLabel != nullptr );
	highIntLabel->SetToLabel();

	JXStaticText* intRangeLabel =
		jnew JXStaticText(JGetString("intRangeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,80, 70,20);
	assert( intRangeLabel != nullptr );
	intRangeLabel->SetToLabel();

	JXStaticText* intLabel =
		jnew JXStaticText(JGetString("intLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,20, 70,20);
	assert( intLabel != nullptr );
	intLabel->SetToLabel();

	JXPasswordInput* pwInput =
		jnew JXPasswordInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,110, 180,20);
	assert( pwInput != nullptr );

	JXStaticText* pwLabel =
		jnew JXStaticText(JGetString("pwLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 100,20);
	assert( pwLabel != nullptr );
	pwLabel->SetToLabel();

	JXStaticText* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 300,110, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	JXStaticText* fileLabel =
		jnew JXStaticText(JGetString("fileLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 300,130, 40,20);
	assert( fileLabel != nullptr );
	fileLabel->SetToLabel();

	JXStaticText* charLabel =
		jnew JXStaticText(JGetString("charLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,130, 100,20);
	assert( charLabel != nullptr );
	charLabel->SetToLabel();

	JXCharInput* charInput =
		jnew JXCharInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,130, 180,20);
	assert( charInput != nullptr );

	JXPathInput* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,110, 260,20);
	assert( pathInput != nullptr );

	JXFileInput* fileInput =
		jnew JXFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,130, 260,20);
	assert( fileInput != nullptr );

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

	pathInput->SetBasePath(JGetCurrentDirectory());
	pathInput->GetText()->SetText(JString("~", false));

	fileInput->SetBasePath(JGetCurrentDirectory());
	fileInput->GetText()->SetText(JString("./testjx", false));

	charInput->SetCharacter(JUtf8Character("\xC3\xA7"));
}

/******************************************************************************
 OKToDeactivate (virtual protected)

	Demonstrates how to add extra restrictions to input fields.

 ******************************************************************************/

bool
TestInputFieldsDialog::OKToDeactivate()
{
JInteger v1,v2;

	if (!JXDialogDirector::OKToDeactivate())
		{
		return false;
		}
	else if (Cancelled())
		{
		return true;
		}

	else if (itsLowerValue->GetValue(&v1) &&
			 itsUpperValue->GetValue(&v2) &&
			 v1 >= v2)
		{
		JGetUserNotification()->ReportError(JGetString("MinMaxError::TestInputFieldsDialog"));
		itsLowerValue->Focus();
		return false;
		}

	else
		{
		return true;
		}
}
