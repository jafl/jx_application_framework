/******************************************************************************
 GFGPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 2002 by New Planet Software. All rights reserved.

 ******************************************************************************/

#include "GFGPrefsDialog.h"

#include <JXChooseSaveFile.h>
#include <JXHelpManager.h>
#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXWindow.h>

#include <JColormap.h>
#include <JString.h>

#include <jXGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GFGPrefsDialog::GFGPrefsDialog
	(
	JXDirector*	supervisor,
	const JString& header,
	const JString& source,
	const JString& constructor,
	const JString& destructor,
	const JString& function
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(header, source, constructor, destructor, function);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GFGPrefsDialog::~GFGPrefsDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GFGPrefsDialog::BuildWindow
	(
	const JString& header,
	const JString& source,
	const JString& constructor,
	const JString& destructor,
	const JString& function
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 510,530, "");
	assert( window != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 470,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsHeaderInput =
		new JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 470,60);
	assert( itsHeaderInput != NULL );

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,115, 470,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsSourceInput =
		new JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,135, 470,60);
	assert( itsSourceInput != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,210, 470,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	itsConstructorInput =
		new JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,230, 470,60);
	assert( itsConstructorInput != NULL );

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,305, 470,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	itsDestructorInput =
		new JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,325, 470,60);
	assert( itsDestructorInput != NULL );

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,400, 470,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

	itsFunctionInput =
		new JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,420, 470,60);
	assert( itsFunctionInput != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 110,500, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::GFGPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 330,500, 70,20);
	assert( okButton != NULL );

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 220,500, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::GFGPrefsDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GFGPrefsDialog"));
	SetButtons(okButton, cancelButton);

	itsHeaderInput->SetIsRequired();
    itsSourceInput->SetIsRequired();
    itsConstructorInput->SetIsRequired();
    itsDestructorInput->SetIsRequired();
    itsFunctionInput->SetIsRequired();

    itsHeaderInput->SetText(header);
    itsSourceInput->SetText(source);
    itsConstructorInput->SetText(constructor);
    itsDestructorInput->SetText(destructor);
    itsFunctionInput->SetText(function);

}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
GFGPrefsDialog::GetValues
	(
	JString* header, 
	JString* source,
	JString* constructor, 
	JString* destructor,
	JString* function
	)
{
	*header			= itsHeaderInput->GetText();
	*source			= itsSourceInput->GetText();
	*constructor	= itsConstructorInput->GetText();
	*destructor		= itsDestructorInput->GetText();
	*function		= itsFunctionInput->GetText();
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 *****************************************************************************/

JBoolean
GFGPrefsDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return JXDialogDirector::OKToDeactivate();
		}

	JBoolean ok	= JXDialogDirector::OKToDeactivate();
	if (ok)
		{
		if (!itsHeaderInput->InputValid() ||
			!itsSourceInput->InputValid() ||
			!itsConstructorInput->InputValid() ||
			!itsDestructorInput->InputValid() ||
			!itsFunctionInput->InputValid())
			{
			ok	= kJFalse;
			}
		}

	return ok;
}

