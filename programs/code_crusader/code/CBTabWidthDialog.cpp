/******************************************************************************
 CBTabWidthDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2004 by John Lindal.

 ******************************************************************************/

#include "CBTabWidthDialog.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBTabWidthDialog::CBTabWidthDialog
	(
	JXWindowDirector*	supervisor,
	const JSize			tabWidth
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(tabWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTabWidthDialog::~CBTabWidthDialog()
{
}

/******************************************************************************
 GetTabCharCount

 ******************************************************************************/

JSize
CBTabWidthDialog::GetTabCharCount()
	const
{
	JInteger value;
	itsTabWidthInput->GetValue(&value);
	return value;
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBTabWidthDialog::BuildWindow
	(
	const JSize tabWidth
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 180,90, JString::empty);
	assert( window != nullptr );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBTabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,60, 60,20);
	assert( cancelButton != nullptr );

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::CBTabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,60, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::CBTabWidthDialog::shortcuts::JXLayout"));

	itsTabWidthInput =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 40,20);
	assert( itsTabWidthInput != nullptr );

	JXStaticText* tabWidthLabel =
		jnew JXStaticText(JGetString("tabWidthLabel::CBTabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 100,20);
	assert( tabWidthLabel != nullptr );
	tabWidthLabel->SetToLabel();

// end JXLayout

	window->SetTitle("Tab width");
	SetButtons(okButton, cancelButton);

	itsTabWidthInput->SetValue(tabWidth);
	itsTabWidthInput->SetLowerLimit(1);
}
