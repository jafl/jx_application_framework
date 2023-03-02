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

	auto* window = jnew JXWindow(this, 510,570, JString::empty);
	assert( window != nullptr );

	auto* noButton =
		jnew JXTextButton(JGetString("noButton::JXAcceptLicenseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 310,540, 100,20);
	assert( noButton != nullptr );

	auto* yesButton =
		jnew JXTextButton(JGetString("yesButton::JXAcceptLicenseDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,540, 100,20);
	assert( yesButton != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 470,500);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXAcceptLicenseDialog"));

	SetButtons(yesButton, noButton);

	auto* text =
		jnew JXStaticText(JGetString("LICENSE"), true, false, true,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( text != nullptr );
	text->FitToEnclosure();
}
