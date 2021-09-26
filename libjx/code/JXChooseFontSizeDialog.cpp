/******************************************************************************
 JXChooseFontSizeDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXChooseFontSizeDialog.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXTextButton.h"
#include "jx-af/jx/JXIntegerInput.h"
#include "jx-af/jx/JXStaticText.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXChooseFontSizeDialog::JXChooseFontSizeDialog
	(
	JXWindowDirector*	supervisor,
	const JSize			fontSize
	)
	:
	JXDialogDirector(supervisor, true)
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

	auto* window = jnew JXWindow(this, 190,90, JString::empty);
	assert( window != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JXChooseFontSizeDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 110,60, 60,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JXChooseFontSizeDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JXChooseFontSizeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 60,20);
	assert( cancelButton != nullptr );

	itsFontSize =
		jnew JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 120,20, 40,20);
	assert( itsFontSize != nullptr );

	auto* fontSizeLabel =
		jnew JXStaticText(JGetString("fontSizeLabel::JXChooseFontSizeDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 110,20);
	assert( fontSizeLabel != nullptr );
	fontSizeLabel->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXChooseFontSizeDialog"));
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
	const bool ok = itsFontSize->GetValue(&size);
	assert( ok );
	return size;
}
