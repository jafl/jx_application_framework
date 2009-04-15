/******************************************************************************
 SCAddFnDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
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
    SetWindow(window);

    itsVarName =
        new JXExprInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 110,15, 200,20);
    assert( itsVarName != NULL );

    JXExprEditorSet* obj1 =
        new JXExprEditorSet(varList, &itsExprWidget, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,50, 330,130);
    assert( obj1 != NULL );

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 50,200, 70,20);
    assert( cancelButton != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 199,199, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXStaticText* obj2 =
        new JXStaticText("Variable name:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,15, 100,20);
    assert( obj2 != NULL );

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
