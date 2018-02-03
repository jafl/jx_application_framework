/******************************************************************************
 SCAddFnDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "SCAddFnDialog.h"
#include "SCCircuitVarList.h"

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXExprEditorSet.h>
#include <JXExprEditor.h>
#include <JXExprInput.h>
#include <jXGlobals.h>

#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCAddFnDialog::SCAddFnDialog
	(
	JXWindowDirector*	supervisor,
	SCCircuitVarList*	varList
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsVarList = varList;
	BuildWindow(varList);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCAddFnDialog::~SCAddFnDialog()
{
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
SCAddFnDialog::BuildWindow
	(
	const SCCircuitVarList* varList
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 330,230, "");
	assert( window != NULL );

	itsVarName =
		new JXExprInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 110,15, 200,20);
	assert( itsVarName != NULL );

	JXExprEditorSet* obj1_JXLayout =
		new JXExprEditorSet(varList, &itsExprWidget, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,50, 330,130);
	assert( obj1_JXLayout != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SCAddFnDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 50,200, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::SCAddFnDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,200, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::SCAddFnDialog::shortcuts::JXLayout"));

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::SCAddFnDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,15, 100,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

// end JXLayout

	window->SetTitle("Add new function");
	SetButtons(okButton, cancelButton);

	itsVarName->SetIsRequired();
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
SCAddFnDialog::OKToDeactivate()
{
	if (!JXDialogDirector::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	else if (itsExprWidget->ContainsUIF())
		{
		(JGetUserNotification())->ReportError("Please finish entering the function.");
		itsExprWidget->Focus();
		return kJFalse;
		}

	const JString varName = itsVarName->GetVarName();
	if (!itsVarList->AddFunction(varName, *(itsExprWidget->GetFunction()), kJTrue))
		{
		itsVarName->Focus();
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}
