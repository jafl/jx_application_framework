/******************************************************************************
 GFGPrefsDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 2002 by New Planet Software. All rights reserved.

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

	JXWindow* window = jnew JXWindow(this, 510,530, "");
	assert( window != NULL );

	JXStaticText* headerCommentLabel =
		jnew JXStaticText(JGetString("headerCommentLabel::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 470,20);
	assert( headerCommentLabel != NULL );
	headerCommentLabel->SetToLabel();

	itsHeaderInput =
		jnew JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 470,60);
	assert( itsHeaderInput != NULL );

	JXStaticText* sourceCommentLabel =
		jnew JXStaticText(JGetString("sourceCommentLabel::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,115, 470,20);
	assert( sourceCommentLabel != NULL );
	sourceCommentLabel->SetToLabel();

	itsSourceInput =
		jnew JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,135, 470,60);
	assert( itsSourceInput != NULL );

	JXStaticText* ctorCommentLabel =
		jnew JXStaticText(JGetString("ctorCommentLabel::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,210, 470,20);
	assert( ctorCommentLabel != NULL );
	ctorCommentLabel->SetToLabel();

	itsConstructorInput =
		jnew JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,230, 470,60);
	assert( itsConstructorInput != NULL );

	JXStaticText* dtorCommentLabel =
		jnew JXStaticText(JGetString("dtorCommentLabel::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,305, 470,20);
	assert( dtorCommentLabel != NULL );
	dtorCommentLabel->SetToLabel();

	itsDestructorInput =
		jnew JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,325, 470,60);
	assert( itsDestructorInput != NULL );

	JXStaticText* fnCommentLabel =
		jnew JXStaticText(JGetString("fnCommentLabel::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,400, 470,20);
	assert( fnCommentLabel != NULL );
	fnCommentLabel->SetToLabel();

	itsFunctionInput =
		jnew JXInputField(kJFalse, kJTrue, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,420, 470,60);
	assert( itsFunctionInput != NULL );

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 110,500, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::GFGPrefsDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::GFGPrefsDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 330,500, 70,20);
	assert( okButton != NULL );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::GFGPrefsDialog::JXLayout"), window,
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

