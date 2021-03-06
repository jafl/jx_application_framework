/******************************************************************************
 JXChooseFontSizeDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXChooseFontSizeDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseFontSizeDialog::JXChooseFontSizeDialog
	(
	JXWindowDirector*	supervisor,
	const JSize			fontSize
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(fontSize);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXChooseFontSizeDialog::~JXChooseFontSizeDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
JXChooseFontSizeDialog::BuildWindow
	(
	const JSize fontSize
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 190,90, "");
	assert( window != NULL );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JXChooseFontSizeDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,60, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JXChooseFontSizeDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXChooseFontSizeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 60,20);
	assert( cancelButton != NULL );

	itsFontSize =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 120,20, 40,20);
	assert( itsFontSize != NULL );

	JXStaticText* fontSizeLabel =
		jnew JXStaticText(JGetString("fontSizeLabel::JXChooseFontSizeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 110,20);
	assert( fontSizeLabel != NULL );
	fontSizeLabel->SetToLabel();

// end JXLayout

	window->SetTitle("Choose font size");
	SetButtons(okButton, cancelButton);

	itsFontSize->SetValue(fontSize);
	itsFontSize->SetLimits(1, 500);
}

/******************************************************************************
 GetFontSize

 ******************************************************************************/

JSize
JXChooseFontSizeDialog::GetFontSize()
	const
{
	JInteger size;
	const JBoolean ok = itsFontSize->GetValue(&size);
	assert( ok );
	return size;
}
