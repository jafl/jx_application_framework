/******************************************************************************
 MDPrefsDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "MDPrefsDialog.h"
#include "mdGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXInputField.h>
#include <JFontManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

MDPrefsDialog::MDPrefsDialog
	(
	JXDirector*		supervisor,
	const JString&	openCmd
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(openCmd);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MDPrefsDialog::~MDPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
MDPrefsDialog::BuildWindow
	(
	const JString& openCmd
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 420,100, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 90,70, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::MDPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kVElastic, 260,70, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::MDPrefsDialog::shortcuts::JXLayout"));

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::MDPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,40, 270,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetFontSize(8);
	obj2_JXLayout->SetToLabel();

	itsOpenFileInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,20, 270,20);
	assert( itsOpenFileInput != NULL );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::MDPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsOpenFileInput->SetText(openCmd);
	itsOpenFileInput->SetIsRequired();
	itsOpenFileInput->SetFont(window->GetFontManager()->GetDefaultMonospaceFont());
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
MDPrefsDialog::GetValues
	(
	JString* openCmd
	)
	const
{
	*openCmd = itsOpenFileInput->GetText();
}
