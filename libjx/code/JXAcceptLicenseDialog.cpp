/******************************************************************************
 JXAcceptLicenseDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXAcceptLicenseDialog.h"
#include "JXWindow.h"
#include "JXTextButton.h"
#include "JXScrollbarSet.h"
#include "JXStaticText.h"
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAcceptLicenseDialog::JXAcceptLicenseDialog()
	:
	JXModalDialogDirector(true)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXAcceptLicenseDialog::~JXAcceptLicenseDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXAcceptLicenseDialog::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 510,570, JGetString("WindowTitle::JXAcceptLicenseDialog::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 470,500);
	assert( scrollbarSet != nullptr );

	auto* text =
		jnew JXStaticText(JGetString("LICENSE"), true, false, true, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 470,500);
	assert( text != nullptr );

	auto* yesButton =
		jnew JXTextButton(JGetString("yesButton::JXAcceptLicenseDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,540, 100,20);
	assert( yesButton != nullptr );

	auto* noButton =
		jnew JXTextButton(JGetString("noButton::JXAcceptLicenseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 310,540, 100,20);
	assert( noButton != nullptr );

// end JXLayout

	SetButtons(yesButton, noButton);
}
