/******************************************************************************
 SCEditVarValueDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCEditVarValueDialog.h"
#include "SCCircuitVarList.h"

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXFloatInput.h>

#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCEditVarValueDialog::SCEditVarValueDialog
	(
	JXWindowDirector*	supervisor,
	SCCircuitVarList*	varList,
	const JIndex		varIndex
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsVarIndex(varIndex)
{
	itsVarList = varList;
	BuildWindow(itsVarList, itsVarIndex);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCEditVarValueDialog::~SCEditVarValueDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SCEditVarValueDialog::BuildWindow
	(
	SCCircuitVarList*	varList,
	const JIndex		varIndex
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 210,90, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SCEditVarValueDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,55, 60,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SCEditVarValueDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,55, 60,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SCEditVarValueDialog::shortcuts::JXLayout"));

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::SCEditVarValueDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 80,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	itsValue =
		new JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 100,20, 90,20);
	assert( itsValue != NULL );

// end JXLayout

	window->SetTitle("Change value");
	SetButtons(okButton, cancelButton);

	itsValue->SetValue(varList->GetValue(varIndex));
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
SCEditVarValueDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	else
		{
		JFloat value;
		JBoolean ok = itsValue->GetValue(&value);
		assert( ok );
		ok = itsVarList->SetValue(itsVarIndex, value);
		assert( ok );
		return kJTrue;
		}
}
