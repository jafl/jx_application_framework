/******************************************************************************
 TestInputFieldsDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestInputFieldsDialog.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXIntegerInput.h>
#include <jx-af/jx/JXPasswordInput.h>
#include <jx-af/jx/JXCharInput.h>
#include <jx-af/jx/JXPathInput.h>
#include <jx-af/jx/JXFileInput.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestInputFieldsDialog::TestInputFieldsDialog()
	:
	JXModalDialogDirector()
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

	auto* window = jnew JXWindow(this, 620,210, JString::empty);

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,180, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,180, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::TestInputFieldsDialog::shortcuts::JXLayout"));

	auto* text1 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,20, 180,20);
	assert( text1 != nullptr );
	const JFontStyle text1_style(true, true, 0, false, JColorManager::GetRedColor());
	text1->SetFontStyle(text1_style);

	auto* text2 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,40, 180,20);
	assert( text2 != nullptr );
	text2->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	text2->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	const JFontStyle text2_style(true, false, 0, false, JColorManager::GetGreenColor());
	text2->SetFontStyle(text2_style);

	auto* text3 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,60, 180,20);
	assert( text3 != nullptr );
	text3->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	const JFontStyle text3_style(true, true, 0, false, JColorManager::GetBlackColor());
	text3->SetFontStyle(text3_style);

	auto* text4 =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,80, 180,20);
	assert( text4 != nullptr );
	text4->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	const JFontStyle text4_style(false, true, 0, false, JColorManager::GetBlackColor());
	text4->SetFontStyle(text4_style);

	auto* f1 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,20, 90,20);
	assert( f1 != nullptr );

	auto* f2 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,40, 90,20);
	assert( f2 != nullptr );

	auto* f3 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,60, 90,20);
	assert( f3 != nullptr );

	auto* f4 =
		jnew JXFloatInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 380,80, 90,20);
	assert( f4 != nullptr );

	auto* i1 =
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

	auto* i4 =
		jnew JXIntegerInput(window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,80, 50,20);
	assert( i4 != nullptr );

	auto* stringLabel =
		jnew JXStaticText(JGetString("stringLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 100,20);
	assert( stringLabel != nullptr );
	stringLabel->SetToLabel();

	auto* reqStringLabel =
		jnew JXStaticText(JGetString("reqStringLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 100,20);
	assert( reqStringLabel != nullptr );
	reqStringLabel->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	reqStringLabel->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	const JFontStyle reqStringLabel_style(true, false, 0, false, JColorManager::GetGreenColor());
	reqStringLabel->SetFontStyle(reqStringLabel_style);
	reqStringLabel->SetToLabel();

	auto* max5Label =
		jnew JXStaticText(JGetString("max5Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,60, 100,20);
	assert( max5Label != nullptr );
	max5Label->SetFontName(JGetString("TimesFontName::TestInputFieldsDialog::JXLayout"));
	const JFontStyle max5Label_style(true, true, 0, false, JColorManager::GetBlackColor());
	max5Label->SetFontStyle(max5Label_style);
	max5Label->SetToLabel();

	auto* max10Label =
		jnew JXStaticText(JGetString("max10Label::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,80, 100,20);
	assert( max10Label != nullptr );
	max10Label->SetFontSize(JFontManager::GetDefaultFontSize()-2);
	const JFontStyle max10Label_style(false, true, 0, false, JColorManager::GetBlackColor());
	max10Label->SetFontStyle(max10Label_style);
	max10Label->SetToLabel();

	auto* numberLabel =
		jnew JXStaticText(JGetString("numberLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,20, 80,20);
	assert( numberLabel != nullptr );
	numberLabel->SetToLabel();

	auto* positiveLabel =
		jnew JXStaticText(JGetString("positiveLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,40, 80,20);
	assert( positiveLabel != nullptr );
	positiveLabel->SetToLabel();

	auto* negativeLabel =
		jnew JXStaticText(JGetString("negativeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,60, 80,20);
	assert( negativeLabel != nullptr );
	negativeLabel->SetToLabel();

	auto* rangeLabel =
		jnew JXStaticText(JGetString("rangeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 300,80, 80,20);
	assert( rangeLabel != nullptr );
	rangeLabel->SetToLabel();

	auto* lowIntLabel =
		jnew JXStaticText(JGetString("lowIntLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,40, 70,20);
	assert( lowIntLabel != nullptr );
	lowIntLabel->SetToLabel();

	auto* highIntLabel =
		jnew JXStaticText(JGetString("highIntLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,60, 70,20);
	assert( highIntLabel != nullptr );
	highIntLabel->SetToLabel();

	auto* intRangeLabel =
		jnew JXStaticText(JGetString("intRangeLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,80, 70,20);
	assert( intRangeLabel != nullptr );
	intRangeLabel->SetToLabel();

	auto* intLabel =
		jnew JXStaticText(JGetString("intLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 480,20, 70,20);
	assert( intLabel != nullptr );
	intLabel->SetToLabel();

	auto* pwInput =
		jnew JXPasswordInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,110, 180,20);
	assert( pwInput != nullptr );

	auto* pwLabel =
		jnew JXStaticText(JGetString("pwLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 100,20);
	assert( pwLabel != nullptr );
	pwLabel->SetToLabel();

	auto* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 300,110, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	auto* fileLabel =
		jnew JXStaticText(JGetString("fileLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 300,130, 40,20);
	assert( fileLabel != nullptr );
	fileLabel->SetToLabel();

	auto* charLabel =
		jnew JXStaticText(JGetString("charLabel::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,130, 100,20);
	assert( charLabel != nullptr );
	charLabel->SetToLabel();

	auto* charInput =
		jnew JXCharInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 110,130, 180,20);
	assert( charInput != nullptr );

	itsPathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,110, 210,20);
	assert( itsPathInput != nullptr );

	itsFileInput =
		jnew JXFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 340,130, 210,20);
	assert( itsFileInput != nullptr );

	itsChoosePathButton =
		jnew JXTextButton(JGetString("itsChoosePathButton::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 550,110, 50,20);
	assert( itsChoosePathButton != nullptr );

	itsChooseFileButton =
		jnew JXTextButton(JGetString("itsChooseFileButton::TestInputFieldsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 550,130, 50,20);
	assert( itsChooseFileButton != nullptr );

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

	itsPathInput->SetBasePath(JGetCurrentDirectory());
	itsPathInput->GetText()->SetText("~");
	ListenTo(itsChoosePathButton, std::function([this](const JXButton::Pushed&)
	{
		itsPathInput->ChoosePath();
	}));

	itsFileInput->SetBasePath(JGetCurrentDirectory());
	itsFileInput->GetText()->SetText("./testjx");
	ListenTo(itsChooseFileButton, std::function([this](const JXButton::Pushed&)
	{
		itsFileInput->ChooseFile();
	}));

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

	if (!JXModalDialogDirector::OKToDeactivate())
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
