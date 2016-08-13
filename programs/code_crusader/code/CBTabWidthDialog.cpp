/******************************************************************************
 CBTabWidthDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 2004 by John Lindal. All rights reserved.

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

	JXWindow* window = new JXWindow(this, 180,90, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::CBTabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,60, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::CBTabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,60, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::CBTabWidthDialog::shortcuts::JXLayout"));

	itsTabWidthInput =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,20, 40,20);
	assert( itsTabWidthInput != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::CBTabWidthDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 100,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Tab width");
	SetButtons(okButton, cancelButton);

	itsTabWidthInput->SetValue(tabWidth);
	itsTabWidthInput->SetLowerLimit(1);
}
