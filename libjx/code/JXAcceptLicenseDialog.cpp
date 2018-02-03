/******************************************************************************
 JXAcceptLicenseDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXAcceptLicenseDialog.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXAcceptLicenseDialog::JXAcceptLicenseDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJFalse)
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

	JXWindow* window = jnew JXWindow(this, 510,570, JString::empty);
	assert( window != NULL );

	JXTextButton* noButton =
		jnew JXTextButton(JGetString("noButton::JXAcceptLicenseDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 310,540, 100,20);
	assert( noButton != NULL );

	JXTextButton* yesButton =
		jnew JXTextButton(JGetString("yesButton::JXAcceptLicenseDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 100,540, 100,20);
	assert( yesButton != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 470,500);
	assert( scrollbarSet != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::JXAcceptLicenseDialog"));
	window->LockCurrentMinSize();
	window->PlaceAsDialogWindow();

	SetButtons(yesButton, noButton);

	JXStaticText* text =
		jnew JXStaticText(JGetString("LICENSE"), kJTrue, kJFalse,
						 scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						 JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( text != NULL );
	text->FitToEnclosure();
}
